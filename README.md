# pbjson
A protobuf plugin for json schema!

# 功能
- 解析PB文件，生成json配置
- 解析PB注释，增强结构表达

# 用法

- 使用protoc导出

```bat
@echo off
setlocal enabledelayedexpansion

set RootDir=%~dp0
set ProtoDir=%RootDir%\..\bin\proto\

rmdir /Q /S %ProtoDir%
md %ProtoDir%

set Files=
for %%i in (*.proto) do (
        call set "Files=%%i %%Files%%"
)

protoc.exe --plugin=protoc-gen-json=./pbjson.exe --json_out=./ %Files%

pause

```

- proto配置，注释也会被导出，带 @ 符号的会生成特殊的 json object

```proto
//NID_LOGIN_ROLE_CREATE_REQ
//@role_create_req : 123
message login_role_create_req
{
    //@user_id : 234
    uint64 user_id              = 1;        // 账号id
    string name                 = 2;        // 名字
    uint32 gender               = 3;        // 性别
    bytes custom                = 4;        // 捏脸数据，rolemodel
}

```

- 输出示例

```json
{
    "name": "login_role_create_req",
    "full_name": "ncmd_cs.login_role_create_req",
    "extend": {
        "desc": [
            "NID_LOGIN_ROLE_CREATE_REQ"
        ],
        "role_create_req": [
            "123"
        ]
    },
    "alias": "login_role_create_req",
    "fields": [
        {
            "is_repeated": false,
            "type": "TYPE_UINT64",
            "type_name": "uint64",
            "name": "user_id",
            "full_name": "ncmd_cs.login_role_create_req.user_id",
            "number": 1,
            "extend": {
                "desc": [
                    "账号id\n"
                ],
                "user_id": [
                    "234"
                ]
            },
            "alias": "user_id"
        },
        {
            "is_repeated": false,
            "type": "TYPE_STRING",
            "type_name": "string",
            "name": "name",
            "full_name": "ncmd_cs.login_role_create_req.name",
            "number": 2,
            "extend": {
                "desc": [
                    "名字\n"
                ]
            },
            "alias": "name"
        },
        {
            "is_repeated": false,
            "type": "TYPE_UINT32",
            "type_name": "uint32",
            "name": "gender",
            "full_name": "ncmd_cs.login_role_create_req.gender",
            "number": 3,
            "extend": {
                "desc": [
                    "性别\n"
                ]
            },
            "alias": "gender"
        },
        {
            "is_repeated": false,
            "type": "TYPE_BYTES",
            "type_name": "bytes",
            "name": "custom",
            "full_name": "ncmd_cs.login_role_create_req.custom",
            "number": 4,
            "extend": {
                "desc": [
                    "捏脸数据，rolemodel\n"
                ]
            },
            "alias": "custom"
        }
    ]
},

```
