#pragma once

#include <stdio.h>
#include <iostream>
#include <vector>
#include <map>
#include <curl/curlcpp.h>

namespace EaseSDK
{
	typedef enum ERR_CODE
	{
		ERR_SUCCESS = 200,
		ERR_INVALID_GTANT = 400,
		ERR_UNAUTHORIZED = 401,
		ERR_SERVER_REJECTED = 403,
		ERR_NOT_FOUND = 404,
		ERR_REQUEST_ENTITY_TOO_LARGE = 413,
		ERR_INTERNAL_SERVER_ERROR = 500,
		ERR_NOT_IMPLEMENTED = 501,
		ERR_BAD_GATEWAY = 502,
		ERR_SERVICE_UNAVAILABLE = 503,
		ERR_GATEWAY_TIMEOUT = 504,
	};

	typedef enum MESSAGE_TYPE
	{
		MESSAGE_TYPE_TEXT,
		MESSAGE_TYPE_IMG,
		MESSAGE_TYPE_AUDIO,
		MESSAGE_TYPE_VIDEO,
	};

	typedef struct _TOKEN
	{
		std::string		access_token;
		int				expires_in;
		std::string		application;
	} TOKEN;

	typedef struct _UPLOAD_FILE
	{
		bool			restrict_access;
		std::string		authorization;
		std::string		file_path;
	} UPLOAD_FILE;

	typedef struct _ENTITY
	{
		std::string		uuid;
		std::string		type;
		std::string		share_secret;
	} ENTITY;

	// 上传返回结果
	typedef struct _UPLOAD_RESULT
	{
		std::string				action;
		std::string				application;
		std::string				path;
		std::string				uri;
		__int64					timestamp;
		int						duration;
		std::string				organization;
		std::string				applicationName;
		std::vector<ENTITY>		entities;

	} UPLOAD_RESULT;

	// 聊天记录
	typedef struct _CHAT_RECORD
	{
		std::string							type;
		std::string							from;
		std::string							msg_id;
		std::string							chat_type;
		std::string							msg;
		std::string							msg_type;
		int									length;
		std::string							url;
		std::string							filename;
		std::string							secret;
		double								lat;
		double								lng;
		std::string							addr;
		std::map<std::string, std::string>	ext;
		std::string							timestamp;
		std::string							to;
	} CHAT_RECORD;

	// 消息基类
	typedef struct _MESSAGE_BASE
	{
		std::string							target_type;
		std::vector<std::string>			target;
		std::string							type;
		std::string							from;
		std::map<std::string, std::string>	ext;
	} MESSAGE_BASE;

	// 文本消息
	typedef struct _TEXT_MESSAGE : public MESSAGE_BASE
	{
		_TEXT_MESSAGE() { type = "text";}
		std::string		msg;
	} TEXT_MESSAGE;

	typedef struct _SIZE
	{
		int		width;
		int		height;

	} SIZE;

	// 图片类型消息
	typedef struct _IMG_MESSAGE : public MESSAGE_BASE
	{
		_IMG_MESSAGE() { type = "img"; }
		std::string		type;
		std::string		url;
		std::string		filename;
		std::string		secret;
		SIZE			size;
	} IMG_MESSAGE;
	
	// 语音消息
	typedef struct _AUDIO_MESSAGE : public MESSAGE_BASE
	{
		_AUDIO_MESSAGE() { type = "audio"; }
		std::string		url;
		std::string		filename;
		int				length;
		std::string		secret;
	} AUDIO_MESSAGE;

	// 地址位置消息
	typedef struct _LOC_MESSAGE
	{
		std::string		from;
		std::string		to;
		struct BODIES
		{
			std::string		type;
			std::string		addr;
			double			lat;
			double			lng;
		};
		BODIES			bodies;
	} LOC_MESSAGE;

	// 视频消息
	typedef struct _VIDEO_MESSAGE : public MESSAGE_BASE
	{
		_VIDEO_MESSAGE() { type = "video"; }
		std::string		filename;
		std::string		thumb;
		std::string		length;
		std::string		secret;
		std::string		file_length;
		std::string		thumb_secret;
		std::string		url;
		
	} VIDEO_MESSAGE;

	//////////////////////////////////////////////////////////////////////////
	// 获取Token
	int GetToken(TOKEN& token);
	// 注册用户
	int RegisterUser(const std::string username, const std::string password);
	// 获取用户
	int GetUser(const std::string user);
	// 删除用户
	int DeleteUser(const std::string user);
	// 重置密码
	int ResetPassword(const std::string newpwd);
	// 上传文件
	int UploadFile(const UPLOAD_FILE& upload, UPLOAD_RESULT& result);
	// 发送文本消息
	int SendTextMsg(const TOKEN& token, const TEXT_MESSAGE& msg);
	// 发送图片消息
	int SendImgMsg(const TOKEN& token, const IMG_MESSAGE& msg);
	// 发送语音消息
	int SendAudioMsg(const TOKEN& token, const AUDIO_MESSAGE& msg);
	// 发送地址消息
	int SendLocMsg(std::string token, const LOC_MESSAGE& msg);
	// 发送视频消息
	int SendVideoMsg(const TOKEN& token, const VIDEO_MESSAGE& msg);

}

