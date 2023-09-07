#include <stdio.h>
#include "PBExtendHelper.h"
#include <rapidjson/document.h>
#include <rapidjson/error/en.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>

const char* aszType[19] = {
    "TYPE_UNKNOW",
    "TYPE_DOUBLE",
    "TYPE_FLOAT",
    "TYPE_INT64",
    "TYPE_UINT64",
    "TYPE_INT32",
    "TYPE_FIXED64",
    "TYPE_FIXED32",
    "TYPE_BOOL",
    "TYPE_STRING",
    "TYPE_GROUP",
    "TYPE_MESSAGE",
    "TYPE_BYTES",
    "TYPE_UINT32",
    "TYPE_ENUM",
    "TYPE_SFIXED32",
    "TYPE_SFIXED64",
    "TYPE_SINT32",
    "TYPE_SINT64"
};

typedef std::map<std::string, PBExtendHelper*> ExtendMap;
ExtendMap g_stExtendMap;

class PracingGenerator : public CodeGenerator {
public:
    virtual bool GenerateAll(const std::vector<const FileDescriptor*>& files,
            const std::string& parameter, GeneratorContext* generator_context, std::string* error) const {
        GenerateSchema(files, parameter, generator_context, error);
        return true;
    }

    virtual bool Generate(const google::protobuf::FileDescriptor* file,
            const std::string& parameter, GeneratorContext* generator_context, std::string* error) const {
        //m_stExtendMap.clear();
        std::string filename = "output.txt";
        std::unique_ptr<io::ZeroCopyOutputStream> output(generator_context->Open(filename));
        io::Printer printer(output.get(), '^');
        //printer.Print("file: ^file^\n", "file", file->name());
        for (int i = 0; i < file->message_type_count(); i++) {
            const Descriptor* desc = file->message_type(i);
            printer.Print("message: ^msg^\n", "msg", desc->full_name());
            for (int k = 0; k < desc->field_count(); k++) {
                printer.Indent();
                const FieldDescriptor* fdesc = desc->field(k);
                printer.Print("field: ^f^\n", "f", fdesc->full_name());
                printer.Outdent();
            }
        }
        return true;
    }

    bool GenerateSchema(const std::vector<const FileDescriptor*>& files,
                        const std::string& parameter, GeneratorContext* generator_context, std::string* error) const {
        std::string hfile = "pbschema.json";
        std::unique_ptr<io::ZeroCopyOutputStream> oHeader(generator_context->Open(hfile));
        io::Printer hprinter(oHeader.get(), '^');

        rapidjson::StringBuffer s;
        rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(s);
        writer.StartObject();

        std::map<std::string, uint32> fileIdxs;
        std::map<std::string, std::vector<uint32>> enumIdxs;
        std::map<std::string, std::vector<uint32>> messageIdxs;

        writer.Key("files");
        writer.StartArray();
        // const Descriptor* pAutoConfig = NULL;
        for (int n = 0; n < files.size(); n++) {
            writer.StartObject();
            const FileDescriptor* file = files[n];
            fileIdxs[file->name()] = n;
            writer.Key("filename");
            writer.String(file->name().c_str());
            writer.Key("package");
            writer.String(file->package().c_str());
            writer.Key("syntax");
            writer.Int((int)file->syntax());
            writer.Key("dependency");
            writer.StartArray();
            for (int i = 0; i < file->dependency_count(); i++) {
                writer.String(file->dependency(i)->name().c_str());
            }
            writer.EndArray();

            writer.Key("enums");
            writer.StartArray();
            //遍历所有enum类型
            for (int i = 0; i < file->enum_type_count(); i++) {
                const EnumDescriptor* desc = file->enum_type(i);
                enumIdxs[desc->full_name()] = {(uint32)n, (uint32)i};

                writer.StartObject();
                writer.Key("name");
                writer.String(desc->name().c_str());
                writer.Key("full_name");
                writer.String(desc->full_name().c_str());
                writer.Key("extend");
                PBExtendHelper stPBExtend(desc);
                stPBExtend.Dump(writer);
                writer.Key("alias");
                if (stPBExtend.HasProp("name")) {
                    writer.String(stPBExtend.GetProp("name").c_str());
                } else {
                    writer.String(desc->name().c_str());
                }

                writer.Key("values");
                writer.StartArray();
                for (int k = 0; k < desc->value_count(); k++) {
                    // hprinter.Indent();
                    const EnumValueDescriptor* vdesc = desc->value(k);
                    writer.StartObject();

                    writer.Key("name");
                    writer.String(vdesc->name().c_str());
                    writer.Key("full_name");
                    writer.String(vdesc->full_name().c_str());
                    writer.Key("number");
                    writer.Int(vdesc->number());
                    writer.Key("extend");

                    PBExtendHelper stPBExtend(vdesc);
                    stPBExtend.Dump(writer);
                    writer.Key("alias");
                    if (stPBExtend.HasProp("name")) {
                        writer.String(stPBExtend.GetProp("name").c_str());
                    } else {
                        writer.String(vdesc->name().c_str());
                    }
                    writer.EndObject();
                }
                writer.EndArray();
                writer.EndObject();
            }
            writer.EndArray();

            writer.Key("messages");
            writer.StartArray();

            //遍历所有message类型
            for (int i = 0; i < file->message_type_count(); i++) {
                const Descriptor* desc = file->message_type(i);
                messageIdxs[desc->full_name()] = {(uint32)n, (uint32)i};

                writer.StartObject();
                writer.Key("name");
                writer.String(desc->name().c_str());
                writer.Key("full_name");
                writer.String(desc->full_name().c_str());
                writer.Key("extend");
                PBExtendHelper stPBExtend(desc);
                stPBExtend.Dump(writer);
                writer.Key("alias");
                if (stPBExtend.HasProp("name")) {
                    writer.String(stPBExtend.GetProp("name").c_str());
                } else {
                    writer.String(desc->name().c_str());
                }
                writer.Key("fields");
                writer.StartArray();

                for (int k = 0; k < desc->field_count(); k++) {
                    const FieldDescriptor* fdesc = desc->field(k);
                    writer.StartObject();
                    writer.Key("is_repeated");
                    writer.Bool(fdesc->is_repeated());
                    writer.Key("type");
                    writer.String(aszType[fdesc->type()]);
                    writer.Key("type_name");
                    if (fdesc->message_type()) {
                        writer.String(fdesc->message_type()->full_name().c_str());
                    } else if (fdesc->enum_type()) {
                        writer.String(fdesc->enum_type()->full_name().c_str());
                    } else {
                        writer.String(fdesc->type_name());
                    }

                    const OneofDescriptor* odesc = fdesc->containing_oneof();
                    if (odesc) {
                        writer.Key("oneof");
                        writer.StartObject();
                        writer.Key("extend");

                        PBExtendHelper stPBExtend(odesc);
                        stPBExtend.Dump(writer);
                        writer.EndObject();
                    }

                    writer.Key("name");
                    writer.String(fdesc->name().c_str());
                    writer.Key("full_name");
                    writer.String(fdesc->full_name().c_str());
                    writer.Key("number");
                    writer.Int(fdesc->number());
                    writer.Key("extend");

                    PBExtendHelper stPBExtend(fdesc);
                    stPBExtend.Dump(writer);
                    writer.Key("alias");
                    if (stPBExtend.HasProp("name")) {
                        writer.String(stPBExtend.GetProp("name").c_str());
                    } else {
                        writer.String(fdesc->name().c_str());
                    }

                    writer.EndObject();
                }
                writer.EndArray();
                writer.EndObject();
            }
            writer.EndArray();
            writer.EndObject();
        }
        writer.EndArray();

        writer.Key("indexs");
        writer.StartObject();
        writer.Key("file_idxs");
        writer.StartObject();
        for (auto& it : fileIdxs) {
            writer.Key(it.first.c_str());
            writer.Int(it.second);
        }
        writer.EndObject();
        writer.Key("enum_idxs");
        writer.StartObject();
        for (auto& it : enumIdxs) {
            writer.Key(it.first.c_str());
            writer.StartArray();
            for (auto& v : it.second) {
                writer.Int(v);
            }
            writer.EndArray();
        }
        writer.EndObject();
        writer.Key("message_idxs");
        writer.StartObject();
        for (auto& it : messageIdxs) {
            writer.Key(it.first.c_str());
            writer.StartArray();
            for (auto& v : it.second) {
                writer.Int(v);
            }
            writer.EndArray();
        }
        writer.EndObject();
        writer.EndObject();
        writer.EndObject();
        hprinter.Print("^s^", "s", s.GetString());
        return true;
    }

    static std::string GetFileType(std::string sFile) {
        return sFile.substr(sFile.find_last_of('.') + 1);
    }
};

int main(int argc, char** argv) {
    PracingGenerator generator;
    return PluginMain(argc, argv, &generator);
}
