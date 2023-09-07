# pbjson
A protobuf plugin for json schema!

# ����
- ����PB�ļ�������json����
- ����PBע�ͣ���ǿ�ṹ���

# �÷�

- ʹ��protoc����

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

- proto���ã�ע��Ҳ�ᱻ�������� @ ���ŵĻ���������� json object

```proto
//NID_LOGIN_ROLE_CREATE_REQ
//@role_create_req : 123
message login_role_create_req
{
    //@user_id : 234
    uint64 user_id              = 1;        // �˺�id
    string name                 = 2;        // ����
    uint32 gender               = 3;        // �Ա�
    bytes custom                = 4;        // �������ݣ�rolemodel
}

```

- ���ʾ��

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
                    "�˺�id\n"
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
                    "����\n"
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
                    "�Ա�\n"
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
                    "�������ݣ�rolemodel\n"
                ]
            },
            "alias": "custom"
        }
    ]
},

```
