// Copyright (c) 2016-2018 Easy2D - Nomango
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include "easy2d-network.h"
#include "curl/curl.h"
#include <thread>

#pragma comment(lib, "libcurl.lib")

namespace
{
	using namespace easy2d;
	using namespace easy2d::network;

	size_t write_data(void* buffer, size_t size, size_t nmemb, void* userp)
	{
		std::string* recv_buffer = (std::string*)userp;
		size_t total = size * nmemb;

		// add data to the end of recv_buffer
		// write data maybe called more than once in a single request
		recv_buffer->append((char*)buffer, total);

		return  total;
	}

	class Curl
	{
	public:
		Curl()
			: curl_(curl_easy_init())
			, curl_headers_(nullptr)
		{
		}

		~Curl()
		{
			if (curl_)
			{
				curl_easy_cleanup(curl_);
				curl_ = nullptr;
			}

			if (curl_headers_)
			{
				curl_slist_free_all(curl_headers_);
				curl_headers_ = nullptr;
			}
		}

		bool Init(HttpClient* client, Array<std::string> const& headers, std::string const& url, std::string* response_data, std::string* response_header, char* error_buffer)
		{
			if (!SetOption(CURLOPT_ERRORBUFFER, error_buffer))
				return false;
			if (!SetOption(CURLOPT_TIMEOUT, client->GetTimeoutForRead()))
				return false;
			if (!SetOption(CURLOPT_CONNECTTIMEOUT, client->GetTimeoutForConnect()))
				return false;

			const auto ssl_ca_file = client->GetSSLVerification().to_string();
			if (ssl_ca_file.empty()) {
				if (!SetOption(CURLOPT_SSL_VERIFYPEER, 0L))
					return false;
				if (!SetOption(CURLOPT_SSL_VERIFYHOST, 0L))
					return false;
			}
			else {
				if (!SetOption(CURLOPT_SSL_VERIFYPEER, 1L))
					return false;
				if (!SetOption(CURLOPT_SSL_VERIFYHOST, 2L))
					return false;
				if (!SetOption(CURLOPT_CAINFO, ssl_ca_file.c_str()))
					return false;
			}

			if (!SetOption(CURLOPT_NOSIGNAL, 1L))
				return false;
			if (!SetOption(CURLOPT_ACCEPT_ENCODING, ""))
				return false;

			// set request headers
			if (!headers.empty())
			{
				for (const auto& header : headers)
				{
					curl_headers_ = curl_slist_append(curl_headers_, header.c_str());
				}
				if (!SetOption(CURLOPT_HTTPHEADER, curl_headers_))
					return false;
			}

			return SetOption(CURLOPT_URL, url.c_str())
				&& SetOption(CURLOPT_WRITEFUNCTION, write_data)
				&& SetOption(CURLOPT_WRITEDATA, response_data)
				&& SetOption(CURLOPT_HEADERFUNCTION, write_data)
				&& SetOption(CURLOPT_HEADERDATA, response_header);
		}

		bool Perform(long* response_code)
		{
			if (CURLE_OK != curl_easy_perform(curl_))
				return false;

			CURLcode code = curl_easy_getinfo(curl_, CURLINFO_RESPONSE_CODE, response_code);
			if (code != CURLE_OK || !(*response_code >= 200 && *response_code < 300))
			{
				return false;
			}
			return true;
		}

		template <typename ..._Args>
		bool SetOption(CURLoption option, _Args&&... args)
		{
			return CURLE_OK == curl_easy_setopt(curl_, option, std::forward<_Args>(args)...);
		}

	public:
		static inline bool GetRequest(HttpClient* client, HttpRequestPtr const& request, long* response_code, std::string* response_data, std::string* response_header, char* error_buffer)
		{
			Curl curl;
			return curl.Init(client, request->GetHeaders(), request->GetUrl(), response_data, response_header, error_buffer)
				&& curl.SetOption(CURLOPT_FOLLOWLOCATION, true)
				&& curl.Perform(response_code);
		}

		static inline bool PostRequest(HttpClient* client, HttpRequestPtr const& request, long* response_code, std::string* response_data, std::string* response_header, char* error_buffer)
		{
			Curl curl;
			const auto request_data = request->GetData();
			return curl.Init(client, request->GetHeaders(), request->GetUrl(), response_data, response_header, error_buffer)
				&& curl.SetOption(CURLOPT_POST, 1)
				&& curl.SetOption(CURLOPT_POSTFIELDS, request_data.c_str())
				&& curl.SetOption(CURLOPT_POSTFIELDSIZE, request_data.size())
				&& curl.Perform(response_code);
		}

		static inline bool PutRequest(HttpClient* client, HttpRequestPtr const& request, long* response_code, std::string* response_data, std::string* response_header, char* error_buffer)
		{
			Curl curl;
			const auto request_data = request->GetData();
			return curl.Init(client, request->GetHeaders(), request->GetUrl(), response_data, response_header, error_buffer)
				&& curl.SetOption(CURLOPT_CUSTOMREQUEST, "PUT")
				&& curl.SetOption(CURLOPT_POSTFIELDS, request_data.c_str())
				&& curl.SetOption(CURLOPT_POSTFIELDSIZE, request_data.size())
				&& curl.Perform(response_code);
		}

		static inline bool DeleteRequest(HttpClient* client, HttpRequestPtr const& request, long* response_code, std::string* response_data, std::string* response_header, char* error_buffer)
		{
			Curl curl;
			const auto request_data = request->GetData();
			return curl.Init(client, request->GetHeaders(), request->GetUrl(), response_data, response_header, error_buffer)
				&& curl.SetOption(CURLOPT_CUSTOMREQUEST, "DELETE")
				&& curl.SetOption(CURLOPT_FOLLOWLOCATION, true)
				&& curl.Perform(response_code);
		}

	private:
		CURL* curl_;
		curl_slist* curl_headers_;
	};
}

namespace easy2d
{
	namespace network
	{
		HttpClient::HttpClient()
			: app_(nullptr)
			, timeout_for_connect_(30000 /* 30 seconds */)
			, timeout_for_read_(60000 /* 60 seconds */)
		{
		}

		void HttpClient::SetupComponent(Application * app)
		{
			app_ = app;

			::curl_global_init(CURL_GLOBAL_ALL);

			std::thread thread(Closure(this, &HttpClient::NetworkThread));
			thread.detach();
		}

		void HttpClient::DestroyComponent()
		{
			::curl_global_cleanup();
		}

		void HttpClient::Send(HttpRequestPtr const & request)
		{
			if (!request)
				return;

			request_mutex_.lock();
			request_queue_.push(request);
			request_mutex_.unlock();

			sleep_condition_.notify_one();
		}

		void HttpClient::NetworkThread()
		{
			while (true)
			{
				HttpRequestPtr request;
				{
					std::lock_guard<std::mutex> lock(request_mutex_);
					while (request_queue_.empty())
					{
						sleep_condition_.wait(request_mutex_);
					}
					request = request_queue_.front();
					request_queue_.pop();
				}

				HttpResponsePtr response = new (std::nothrow) HttpResponse(request);
				Perform(request, response);

				response_mutex_.lock();
				response_queue_.push(response);
				response_mutex_.unlock();

				if (app_)
				{
					app_->PreformFunctionInMainThread(Closure(this, &HttpClient::DispatchResponseCallback));
				}
			}
		}

		void HttpClient::Perform(HttpRequestPtr const & request, HttpResponsePtr const & response)
		{
			bool ok = false;
			long response_code = 0;
			char error_message[256];

			switch (request->GetType())
			{
			case HttpRequest::Type::Get:
				ok = Curl::GetRequest(this, request, &response_code, response->GetDataPtr(), response->GetHeaderPtr(), error_message);
				break;
			case HttpRequest::Type::Post:
				ok = Curl::PostRequest(this, request, &response_code, response->GetDataPtr(), response->GetHeaderPtr(), error_message);
				break;
			case HttpRequest::Type::Put:
				ok = Curl::PutRequest(this, request, &response_code, response->GetDataPtr(), response->GetHeaderPtr(), error_message);
				break;
			case HttpRequest::Type::Delete:
				ok = Curl::DeleteRequest(this, request, &response_code, response->GetDataPtr(), response->GetHeaderPtr(), error_message);
				break;
			default:
				E2D_ERROR_LOG(L"HttpClient: unknown request type, only GET, POST, PUT or DELETE is supported");
				return;
			}

			response->SetResponseCode(response_code);
			if (!ok)
			{
				response->SetSucceed(false);
				response->SetError(error_message);
			}
			else
			{
				response->SetSucceed(true);
			}
		}

		void HttpClient::DispatchResponseCallback()
		{
			HttpResponsePtr response;

			response_mutex_.lock();
			if (!response_queue_.empty())
			{
				response = response_queue_.front();
				response_queue_.pop();
			}
			response_mutex_.unlock();

			if (response)
			{
				HttpRequestPtr request = response->GetRequest();
				const auto& callback = request->GetResponseCallback();

				if (callback)
				{
					callback(request, response);
				}
			}
		}

	}
}
