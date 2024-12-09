#pragma once
#include <d3d11.h>
#include <vector>
#include "DXcore.h"
#include "mathLibrary.h"

struct STATIC_VERTEX {
    Vec3 pos;
    Vec3 normal;
    Vec3 tangent;
    float tu;
    float tv;
};

class Mesh {
public:
    ID3D11Buffer* indexBuffer;
    ID3D11Buffer* vertexBuffer;
    int indicesSize;
    UINT strides;

    void init(void* vertices, int vertexSizeInBytes, int numVertices, unsigned int* indices, int numIndices, ID3D11Device*& device) {
        D3D11_BUFFER_DESC bd;
        memset(&bd, 0, sizeof(D3D11_BUFFER_DESC));
        bd.Usage = D3D11_USAGE_DEFAULT;
        bd.ByteWidth = sizeof(unsigned int) * numIndices;
        bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
        D3D11_SUBRESOURCE_DATA data;
        memset(&data, 0, sizeof(data));
        data.pSysMem = indices;
        device->CreateBuffer(&bd, &data, &indexBuffer);

        bd.ByteWidth = vertexSizeInBytes * numVertices;
        bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        data.pSysMem = vertices;
        device->CreateBuffer(&bd, &data, &vertexBuffer);

        indicesSize = numIndices;
        strides = vertexSizeInBytes;
    }

    void init(std::vector<STATIC_VERTEX> vertices, std::vector<unsigned int> indices, ID3D11Device* device) {
        init(&vertices[0], sizeof(STATIC_VERTEX), (int)vertices.size(), &indices[0], (int)indices.size(), device);
    }

    void draw(ID3D11DeviceContext* devicecontext) {
        UINT offsets = 0;
        devicecontext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        devicecontext->IASetVertexBuffers(0, 1, &vertexBuffer, &strides, &offsets);
        devicecontext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
        devicecontext->DrawIndexed(indicesSize, 0, 0);
    }
};

//struct Vertex {
//	Vec3 position;
//	Color color;
//};
//
//class Triangle {
//	ID3D11Buffer* vertexBuffer;
//	Vertex vertices[3];
//public:
//	void init(ID3D11Device*& device) {
//		vertices[0].position = Vec3(0, 1.0f, 0);
//		vertices[0].color = Color(0, 1.0f, 0);
//		vertices[1].position = Vec3(-1.0f, -1.0f, 0);
//		vertices[1].color = Color(1.0f, 0, 0);
//		vertices[2].position = Vec3(1.0f, -1.0f, 0);
//		vertices[2].color = Color(0, 0, 1.0f);
//		
//	}
//
//	// Create a buffer in GPU top store vertex
//	void createBuffer(ID3D11Device* &device) {
//		D3D11_BUFFER_DESC bd;
//		bd.Usage = D3D11_USAGE_DEFAULT;
//		bd.CPUAccessFlags = 0;
//		bd.MiscFlags = 0;
//		D3D11_SUBRESOURCE_DATA uploadData;
//		bd.ByteWidth = sizeof(Vertex) * 3;
//		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
//		uploadData.pSysMem = vertices;
//		uploadData.SysMemPitch = 0;
//		uploadData.SysMemSlicePitch = 0;
//		device->CreateBuffer(&bd, &uploadData, &vertexBuffer);
//	}
//	
//	// Draw triangle
//	void draw(ID3D11DeviceContext* &devicecontext) {
//		UINT offsets;
//		offsets = 0;
//		UINT strides = sizeof(Vertex);
//		devicecontext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//		devicecontext->IASetVertexBuffers(0, 1, &vertexBuffer, &strides, &offsets);
//		devicecontext->Draw(3, 0);
//
//	}
//};
