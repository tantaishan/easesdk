#include "EaseSDK.h"
#include <json/json.h>
#include "config.h"

namespace EaseSDK
{
	static std::string ReadFileContent(std::string filepath)
	{
		std::string result;
		FILE* pf = 0;
		char* buffer = 0;
		int size = 0;
		do
		{
			fopen_s(&pf, filepath.c_str(), "rb");
			if (!pf)
				return result;
			fseek(pf, 0, SEEK_END);
			size = ftell(pf);
			fseek(pf, 0, SEEK_SET);
			buffer = new char[size];
			size_t r = fread(buffer, 1, size, pf);
			if (size != r)
			{
				delete[] buffer;
				buffer = 0;
				return result;
			}
		} while (false);
		if (pf)
		{
			fclose(pf);
			pf = 0;
		}
		if (buffer)
		{
			result = std::string(buffer, size);
			delete[] buffer;
			buffer = 0;
		}

		return result;
	}

	static void InitCurl(curlcpp* curl, const std::string url)
	{
		curl->set_url(url);
		curl->enable_error();
		curl->reset_headers();
		curl->add_header(CONFIG::USER_AGENT);
		curl->add_header("Accept: text/html, application/xml;q=0.9, application/xhtml+xml, */*");
		curl->add_header("Accept-Charset: GB2312,utf-8;q=0.7,*;q=0.7");
		curl->add_header("Accept-Language: zh-cn,zh;q=0.5");
		curl->add_header("Content-Type: application/x-www-form-urlencoded;charset=UTF-8");
		curl->add_header("Connection: Keep-Alive");
	}

	static void InitCurl_Json(curlcpp* curl, std::string url)
	{
		curl->set_url(url);
		curl->enable_error();
		curl->reset_headers();
		curl->add_header(CONFIG::USER_AGENT);
		curl->add_header("Accept: text/html, application/xml;q=0.9, application/xhtml+xml, */*");
		curl->add_header("Accept-Charset: GB2312,utf-8;q=0.7,*;q=0.7");
		curl->add_header("Accept-Language: zh-cn,zh;q=0.5");
		curl->add_header("Content-Type: application/json;charset=UTF-8");
		curl->add_header("Connection: Keep-Alive");
	}
	
	int GetToken(TOKEN& token)
	{
		Json::Value body;
		body["grant_type"] = "client_credentials";
		body["client_id"] = CONFIG::APP_CLIENT_ID;
		body["client_secret"] = CONFIG::APP_CLIENT_SECRET;

		curlcpp curl(false);
		//curl.set_url
		InitCurl_Json(&curl, URL::GET_TOKEN);
		//curl.set_post_data(jsnData.toStyledString());
		std::string response;
		int nRet = curl.post(body.toStyledString(), response);
		if (nRet == 0)
		{
			Json::Reader reader;
			Json::Value result;
			if (reader.parse(response, result))
			{
				token.access_token = result["access_token"].asString();
				token.application = result["application"].asString();
				token.expires_in = result["expires_in"].asInt();
			}
		}
		return nRet;
	}

	int RegisterUser(const std::string username, const std::string password)
	{
		int ret = 0;
		Json::Value request;
		request["username"] = username;
		request["password"] = password;

		curlcpp curl(false);
		//curl.set_url
		InitCurl_Json(&curl, URL::USERS);

		std::string response;
		ret = curl.post(request.toStyledString(), response);
		if (ret == 0)
		{
			Json::Reader reader;
			Json::Value result;
			if (reader.parse(response, result))
			{

			}
		}
		return ret;
	}

	int GetUser(const std::string user)
	{
		int ret = 0;
		Json::Value request;
		curlcpp curl(false);
		//curl.set_url
		std::string url(URL::USERS);
		url.append("/");
		url.append(user);

		InitCurl_Json(&curl, url);

		std::string response;
		ret = curl.post(request.toStyledString(), response);
		if (ret == 0)
		{
			Json::Reader reader;
			Json::Value result;
			if (reader.parse(response, result))
			{

			}
		}
		return ret;
	}

	int UploadFile(const UPLOAD_FILE& upload, UPLOAD_RESULT& result)
	{
		int nRet = 0;
		//Json::Value request;
		//request["restrict-access"] = upload.restrict_access;
		std::string authorization("Bearer ");
		authorization.append(upload.authorization);
		//request["Authorization"] = authorization;

		curlcpp curl(false);

		//InitCurl(&curl, URL::CHAT_FILES);
		curl.set_url(URL::CHAT_FILES);
		curl.enable_error();
		curl.reset_headers();
		curl.add_header("Content-Type: multipart/form-data");
		curl.add_header("restrict-access", upload.restrict_access ? "true" : "false");
		curl.add_header("Authorization", authorization);
		std::string request = ReadFileContent(upload.file_path);
		std::string response;
		nRet = curl.post_multipart(request, response);
		if (nRet == 0)
		{
			Json::Value json;
			Json::Reader reader;
			if (reader.parse(response, json))
			{
				result.action = json["action"].asString();
				result.application = json["application"].asString();
				result.path = json["path"].asString();
				result.uri = json["uri"].asString();

				Json::Value& entities = json["entities"];
				for (unsigned int i = 0; i < entities.size(); i++)
				{
					ENTITY entity;
					entity.uuid = entities[i]["uuid"].asString();
					entity.type = entities[i]["type"].asString();
					entity.share_secret = entities[i]["share-secret"].asString();
					result.entities.push_back(entity);
				}

				result.timestamp = json["timestamp"].asUInt64();
				result.duration = json["duration"].asInt();
				result.organization = json["organization"].asString();
				result.applicationName = json["applicationName"].asString();
			}
		}
		return nRet;
	}

	int SendTextMsg(const TOKEN& token, const TEXT_MESSAGE& msg)
	{
		int ret = 0;
		Json::Value request;
		request["target_type"] = msg.target_type;
		Json::Value target;
		for (unsigned int i = 0; i < msg.target.size(); i++)
			target.append(msg.target[i]);
		request["target"] = target;

		Json::Value text_msg;
		text_msg["type"] = msg.type;
		text_msg["msg"] = msg.msg;

		request["msg"] = text_msg;
		request["from"] = msg.from;

		curlcpp curl(false);
		//curl.set_url
		InitCurl_Json(&curl, URL::MESSAGES);

		std::string authorization("Bearer ");
		authorization.append(token.access_token);
		curl.add_header("Authorization", authorization);

		//curl.set_post_data(jsnData.toStyledString());
		std::string response;
		ret = curl.post(request.toStyledString(), response);
		if (ret == 0)
		{
			Json::Reader reader;
			Json::Value result;
			if (reader.parse(response, result))
			{

			}
		}
		return ret;
	}

	int SendImgMsg(const TOKEN& token, const IMG_MESSAGE& msg)
	{
		int ret = 0;
		Json::Value request;

		request["target_type"] = msg.target_type;

		Json::Value target;
		for (unsigned int i = 0; i < msg.target.size(); i++)
			target.append(msg.target[i]);
		request["target"] = target;

		Json::Value img_msg;
		img_msg["type"] = msg.type;
		img_msg["url"] = msg.url;
		img_msg["filename"] = msg.filename;
		img_msg["secret"] = msg.secret;
		Json::Value img_size;
		img_size["width"] = msg.size.width;
		img_size["height"] = msg.size.height;
		img_msg["size"] = img_size;

		request["msg"] = img_msg;
		request["from"] = msg.from;

		curlcpp curl(false);
		//curl.set_url
		InitCurl_Json(&curl, URL::MESSAGES);

		std::string authorization("Bearer ");
		authorization.append(token.access_token);
		curl.add_header("Authorization", authorization);

		std::string response;
		ret = curl.post(request.toStyledString(), response);
		if (ret == 0)
		{
			Json::Reader reader;
			Json::Value result;
			if (reader.parse(response, result))
			{

			}
		}
		return ret;
	}

	
	int SendLocMsg(std::string token, const LOC_MESSAGE& msg)
	{
		int ret = 0;
		return ret;
	}

	int SendAudioMsg(const TOKEN& token, const AUDIO_MESSAGE& msg)
	{
		int ret = 0;
		Json::Value request;
		request["target_type"] = "users";
		Json::Value target;
		for (unsigned int i = 0; i < msg.target.size(); i++)
			target.append(msg.target[i]);
		request["target"] = target;

		Json::Value audio_msg;
		audio_msg["type"] = msg.type;

		std::string msg_url(URL::CHAT_FILES);
		msg_url.append("/");
		msg_url.append(msg.url);
		audio_msg["url"] = msg_url;

		audio_msg["filename"] = msg.filename;
		audio_msg["length"] = msg.length;
		audio_msg["secret"] = msg.secret;

		request["msg"] = audio_msg;
		request["from"] = msg.from;

		curlcpp curl(false);
		//curl.set_url
		InitCurl_Json(&curl, URL::MESSAGES);

		std::string authorization("Bearer ");
		authorization.append(token.access_token);
		curl.add_header("Authorization", authorization);

		//curl.set_post_data(jsnData.toStyledString());
		std::string response;
		ret = curl.post(request.toStyledString(), response);
		if (ret == 0)
		{
			Json::Reader reader;
			Json::Value result;
			if (reader.parse(response, result))
			{

			}
		}
		return ret;
	}

	int SendVideoMsg(const TOKEN& token, const VIDEO_MESSAGE& msg)
	{
		int ret = 0;
		Json::Value request;

		request["target_type"] = "users";
		Json::Value target;
		for (unsigned int i = 0; i < msg.target.size(); i++)
			target.append(msg.target[i]);
		request["target"] = target;

		Json::Value video_msg;
		video_msg["type"] = msg.type;
		video_msg["filename"] = msg.filename;
		video_msg["thumb"] = msg.thumb;
		video_msg["length"] = msg.length;
		video_msg["secret"] = msg.secret;
		video_msg["file_length"] = msg.file_length;
		video_msg["thumb_secret"] = msg.thumb_secret;

		std::string chatfiles_url(URL::CHAT_FILES);
		chatfiles_url.append("/");
		chatfiles_url.append(msg.url);

		video_msg["url"] = chatfiles_url;
		
		request["msg"] = video_msg;
		request["from"] = msg.from;

		curlcpp curl(false);
		//curl.set_url
		InitCurl_Json(&curl, URL::MESSAGES);

		std::string authorization("Bearer ");
		authorization.append(token.access_token);
		curl.add_header("Authorization", authorization);

		//curl.set_post_data(jsnData.toStyledString());
		std::string response;
		ret = curl.post(request.toStyledString(), response);
		if (ret == 0)
		{
			Json::Reader reader;
			Json::Value result;
			if (reader.parse(response, result))
			{

			}
		}
		return ret;
	}

}