#include "Resources/ModelLoader.hpp"

#define STBI_ONLY_PNG
//#define STBI_NO_FAILURE_STRINGS
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <string.h>
#include <errno.h>

using namespace Maths;
using namespace Resources;

char* ModelLoader::LoadFile(const char* path, u32* sizeOut)
{
#ifdef _WIN32
	FILE* file;
	fopen_s(&file, path, "rb");
#else
	FILE* file = fopen(path, "rb");
#endif

	if (file == NULL)
	{
		printf("Error - cannot open file %s\n", path);
		printf("Error code : %d\n", errno);
		return NULL;
	}
	u32 size = GetFileSize(file);

	char* result = (char*)(malloc(size * sizeof(char)));
	if (result == NULL)
	{
		printf("Error - failed to allocate %d bytes for file %s\nOut of memory?", size, path);
		return NULL;
	}
	if (sizeOut) *sizeOut = size;
	fread(result, 1, size, file);
	fclose(file);
	return result;
}

bool ModelLoader::SaveFile(const char* path, const u32* data, u32 size)
{
#ifdef _WIN32
	FILE* file;
	fopen_s(&file, path, "wb");
#else
	FILE* file = fopen(path, "wb");
#endif

	if (file == NULL)
	{
		printf("Error - cannot open file %s\n", path);
		printf("Error code : %d\n", errno);
		return false;
	}

	fwrite(data, sizeof(u32), size, file);
	fclose(file);
	return true;
}

u32 ModelLoader::GetFileSize(FILE* in)
{
	fseek(in, 0, SEEK_END);
	u32 result = ftell(in);
	fseek(in, 0, SEEK_SET);
	return result;
}

#ifdef _WIN32

#include <string>
#include <vector>
#include <bit>

bool compareWord(const char* buff, int64_t index, const int64_t maxSize, const char* inputWord)
{
    for (index; index < maxSize && (buff[index] == ' ' || buff[index] == '\t'); index++) {}
    for (int64_t i = 0; inputWord[i] != 0; i++)
    {
        if ((index + i > maxSize) || (inputWord[i] != buff[index + i]))
            return false;
    }
    return true;
}

int countSlash(const char* buff, int64_t index, const int64_t maxSize)
{
    int result = 0;
    while (index < maxSize && buff[index] != ' ' && buff[index] != '\n')
    {
        if (buff[index] == '/')
        {
            if (buff[index + 1] == '/')
                return -1;
            else
                result += 1;
        }
        index++;
    }
    return result;
}

int64_t skipCharSafe(const char* buff, int64_t index, const int64_t maxSize)
{
    while (index < maxSize && buff[index] != ' ' && buff[index] != '\n' && buff[index] != '/')
    {
        index += 1;
    }
    if (buff[index] == ' ')
    {
        while (index < maxSize && buff[index] == ' ')
        {
            index += 1;
        }
        return index;
    }
    return index + 1;
}

int64_t skipChar(const char* buff, int64_t index, const int64_t maxSize, const char cr)
{
    while (index < maxSize && buff[index] != cr)
        index += 1;
    return index + 1;
}

int64_t endLine(const char* buff, int64_t index, const int64_t maxSize)
{
    return skipChar(buff, index, maxSize, '\n');
}

int64_t getInt(const char* buf, int64_t index, const int64_t maxSize)
{
    int64_t value = 0;
    if (index >= maxSize) return value;
    char n = buf[index];
    bool negative = false;
    if (n == '-')
    {
        negative = true;
        index++;
        n = buf[index];
    }
    while (n >= '0' && n <= '9' && index < maxSize)
    {
        value *= 10;
        value += (int64_t)n - '0';
        index++;
        n = buf[index];
    }
    return (negative ? -value : value);
}

float getFloat(const char* buf, int64_t index, const int64_t maxSize)
{
    float value = 0;
    if (index >= maxSize) return value;
    char n = buf[index];
    bool negative = false;
    if (n == '-')
    {
        negative = true;
        index++;
        n = buf[index];
    }
    while (n >= '0' && n <= '9' && index < maxSize)
    {
        value *= 10;
        value += n - '0';
        index++;
        n = buf[index];
    }
    if (index >= maxSize) return (negative ? -value : value);
    if (n != '.') return (negative ? -value : value);
    index++;
    n = buf[index];
    unsigned int decimal = 10;
    while (n >= '0' && n <= '9' && decimal < 100000000u && index < maxSize)
    {
        value = value + (float)(n - '0') / decimal;
        decimal *= 10;
        index++;
        n = buf[index];
    }
    return (negative ? -value : value);
}


std::string getLine(const char* buf, int64_t& index, const int64_t maxSize)
{
    std::string line;
    while (buf[index] != '\r' && buf[index] != '\0' && buf[index] != '\n' && index < maxSize)
    {
        line.push_back(buf[index]);
        index++;
    }
    return line;
}

bool ReadVec3(const char* buff, int64_t index, const int64_t maxSize, Vec3& result)
{
    float a = getFloat(buff, index, maxSize);
    index = skipCharSafe(buff, index, maxSize);
    if (index >= maxSize)
    {
        return false;
    }
    float b = getFloat(buff, index, maxSize);
    index = skipCharSafe(buff, index, maxSize);
    if (index >= maxSize)
    {
        return false;
    }
    float c = getFloat(buff, index, maxSize);
    result = Vec3(a, b, c);
    return true;
}

bool ReadVec2(const char* buff, int64_t index, const int64_t maxSize, Vec2& result)
{
    float a = getFloat(buff, index, maxSize);
    index = skipCharSafe(buff, index, maxSize);
    if (index >= maxSize)
    {
        return false;
    }
    float b = getFloat(buff, index, maxSize);
    result = Vec2(a, b);
    return true;
}

std::vector<TriangleData> faces;
std::vector<Vec3> vertices;
std::vector<Vec3> normals;
std::vector<Vec2> tCoord;

void ReadFace(int64_t& pos, size_t& objIndex, signed char& type, const char* data, const int64_t& size)
{
	if (type == -10)
	{
		type = countSlash(data, pos, size);
	}
	unsigned int v[3] = { 0 };
	unsigned int n[3] = { 0 };
	unsigned int t[3] = { 0 };
	if (type == -1)
	{
		v[0] = (unsigned int)getInt(data, pos, size) - 1;
		pos = skipCharSafe(data, pos, size);
		pos += 1;
		n[0] = (unsigned int)getInt(data, pos, size) - 1;
		pos = skipCharSafe(data, pos, size);
		v[1] = (unsigned int)getInt(data, pos, size) - 1;
		pos = skipCharSafe(data, pos, size);
		pos += 1;
		n[1] = (unsigned int)getInt(data, pos, size) - 1;
		pos = skipCharSafe(data, pos, size);
		v[2] = (unsigned int)getInt(data, pos, size) - 1;
		pos = skipCharSafe(data, pos, size);
		pos += 1;
		n[2] = (unsigned int)getInt(data, pos, size) - 1;
	}
	else if (type == 0)
	{
		v[0] = (unsigned int)getInt(data, pos, size) - 1;
		pos = skipCharSafe(data, pos, size);
		v[1] = (unsigned int)getInt(data, pos, size) - 1;
		pos = skipCharSafe(data, pos, size);
		v[2] = (unsigned int)getInt(data, pos, size) - 1;
	}
	else if (type == 1)
	{
		v[0] = (unsigned int)getInt(data, pos, size) - 1;
		pos = skipCharSafe(data, pos, size);
		t[0] = (unsigned int)getInt(data, pos, size) - 1;
		pos = skipCharSafe(data, pos, size);
		v[1] = (unsigned int)getInt(data, pos, size) - 1;
		pos = skipCharSafe(data, pos, size);
		t[1] = (unsigned int)getInt(data, pos, size) - 1;
		pos = skipCharSafe(data, pos, size);
		v[2] = (unsigned int)getInt(data, pos, size) - 1;
		pos = skipCharSafe(data, pos, size);
		t[2] = (unsigned int)getInt(data, pos, size) - 1;
	}
	else if (type == 2)
	{
		v[0] = (unsigned int)getInt(data, pos, size) - 1;
		pos = skipCharSafe(data, pos, size);
		t[0] = (unsigned int)getInt(data, pos, size) - 1;
		pos = skipCharSafe(data, pos, size);
		n[0] = (unsigned int)getInt(data, pos, size) - 1;
		pos = skipCharSafe(data, pos, size);
		v[1] = (unsigned int)getInt(data, pos, size) - 1;
		pos = skipCharSafe(data, pos, size);
		t[1] = (unsigned int)getInt(data, pos, size) - 1;
		pos = skipCharSafe(data, pos, size);
		n[1] = (unsigned int)getInt(data, pos, size) - 1;
		pos = skipCharSafe(data, pos, size);
		v[2] = (unsigned int)getInt(data, pos, size) - 1;
		pos = skipCharSafe(data, pos, size);
		t[2] = (unsigned int)getInt(data, pos, size) - 1;
		pos = skipCharSafe(data, pos, size);
		n[2] = (unsigned int)getInt(data, pos, size) - 1;
	}
	TriangleData tmp;
	for (int i = 0; i < 3; i++)
	{
		auto& vert = tmp.data[i];
		vert.pos = vertices[v[i]];
		if (!normals.empty()) vert.norm = normals[n[i]];
		if (!tCoord.empty()) vert.uv = tCoord[t[i]];
		vert.uv.y = 1 - vert.uv.y;
	}
	faces.push_back(tmp);
}

signed char Loop(const std::string& str)
{
	faces.clear();
	vertices.clear();
	normals.clear();
	tCoord.clear();
	size_t objIndex = 0;
	int64_t pos = 0;
	signed char type = -10;
	s64 size = str.size();
	const char* data = str.data();
	while (pos < size)
	{
		if (compareWord(data, pos, size, "v "))
		{
			pos = skipCharSafe(data, pos, size);
			float a = getFloat(data, pos, size);
			pos = skipCharSafe(data, pos, size);
			float b = getFloat(data, pos, size);
			pos = skipCharSafe(data, pos, size);
			float c = getFloat(data, pos, size);
			vertices.push_back(Vec3(a, b, c));
		}
		else if (compareWord(data, pos, size, "vn "))
		{
			pos = skipCharSafe(data, pos, size);
			float a = getFloat(data, pos, size);
			pos = skipCharSafe(data, pos, size);
			float b = getFloat(data, pos, size);
			pos = skipCharSafe(data, pos, size);
			float c = getFloat(data, pos, size);
			normals.push_back(Vec3(a, b, c));
		}
		else if (compareWord(data, pos, size, "vt "))
		{
			pos = skipCharSafe(data, pos, size);
			float a = getFloat(data, pos, size);
			pos = skipCharSafe(data, pos, size);
			float b = getFloat(data, pos, size);
			tCoord.push_back(Vec2(a, b));
		}
		else if (compareWord(data, pos, size, "f "))
		{
			pos = skipCharSafe(data, pos, size);
			ReadFace(pos, objIndex, type, data, size);
		}
		pos = endLine(data, pos, size);
	}
	return type;
}

void ModelLoader::CreateModelFile(const char* source, const char* tex, const char* dest)
{
	u32 size;
	char* tmp = LoadFile(source, &size);
	if (tmp == NULL || !size)
	{
		return;
	}
	std::string data;
	data.resize(size);
	memcpy(data.data(), tmp, size);
	free(tmp);
	Loop(data);

	std::vector<u32> output;

	output.push_back((u32)(faces.size()));
	for (u32 i = 0; i < faces.size(); i++)
	{
		TriangleData t = faces[i];
		u32* ptr = reinterpret_cast<u32*>(t.data);
		for (int j = 0; j < 24; j++)
		{
			output.push_back(ptr[j]);
		}
	}

	char* texFile = LoadFile(tex, &size);
	if (texFile == NULL || !size)
	{
		return;
	}

	const bool extra = size & (sizeof(u32) - 1);
	const u32 size2 = size / sizeof(u32);

	output.push_back(size2 + (extra ? 1 : 0));

	const u32* ptr = reinterpret_cast<const u32*>(texFile);
	for (u32 i = 0; i < size2; i++)
	{
		output.push_back(ptr[i]);
	}
	if (extra)
	{
		u32 number;
		u8* numPtr = reinterpret_cast<u8*>(&number);
		for (u32 i = size2 * sizeof(u32); i < size; i++)
		{
			numPtr[i - size2 * sizeof(u32)] = texFile[i];
		}
		output.push_back(number);
	}
	free(texFile);

	SaveFile(dest, output.data(), (u32)(output.size()));
}

#endif

ModelData ModelLoader::ParseModelFile(const char* source, const char* skybox, u32* triCount)
{
	ModelData result;
	u32 size;
	char* data = LoadFile(source, &size);
	if (data == NULL || !size)
	{
		return result;
	}
	u32 len = size / sizeof(u32);
	u32* fData = reinterpret_cast<u32*>(data);
	f32* tData = reinterpret_cast<f32*>(data);
	u32 pos = 0;
	u32 fCount = fData[0];
	pos++;
	Triangle* tris = (Triangle*)(malloc(fCount * sizeof(Triangle)));
	if (triCount != NULL) *triCount = fCount;
	if (tris == NULL || !size)
	{
		printf("Error - failed to allocate %zu bytes\nOut of memory?", fCount * sizeof(Triangle));
		free(data);
		return result;
	}
	for (u32 i = 0; i < fCount; i++)
	{
		Triangle* t = tris + i;
		f32* ptr = reinterpret_cast<f32*>(t->data);
		for (u32 j = 0; j < 24; j++)
		{
			ptr[j] = tData[pos];
			pos++;
		}
	}
	u32 texSize = fData[pos];
	pos++;
	s32 comp;
	assert(pos + texSize <= len);
	u8* texPtr = reinterpret_cast<u8*>(fData + pos);
	u8* texData = stbi_load_from_memory(texPtr, texSize * sizeof(u32), &result.tRes.x, &result.tRes.y, &comp, 4);
	if (texData == NULL)
	{
		printf("Error - failed to load texture: %s\n", stbi_failure_reason());
		free(data);
		return result;
	}

	u8* texData2 = NULL;
	IVec2 tmpRes;
	if (skybox != NULL)
	{
		char* skyBoxData = LoadFile(skybox, &texSize);
		texData2 = stbi_load_from_memory(reinterpret_cast<u8*>(skyBoxData), texSize, &tmpRes.x, &tmpRes.y, &comp, 4);
		free(skyBoxData);
		if (texData2 == NULL)
		{
			printf("Error - failed to load skybox texture: %s\n", stbi_failure_reason());
		}
		if (tmpRes.x * 6 != tmpRes.y)
		{
			printf("Error - skybox texture has incorrect size: [%d;%d]\nHeight should be 6 * Width", tmpRes.x, tmpRes.y);
			stbi_image_free(texData2);
			texData2 = NULL;
		}
	}
	
	result.faces = tris;
	result.tex = reinterpret_cast<u32*>(texData);
	result.sky = reinterpret_cast<u32*>(texData2);
	result.sRes = tmpRes;
	free(data);
	return result;
}

void Resources::ModelLoader::FreeImageData(u32* data)
{
	stbi_image_free(data);
}