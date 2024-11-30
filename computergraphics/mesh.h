#pragma once
#include "mathLibrary.h"
#include <d3d11.h>

struct Vertex {
	Vec3 position;
	Color color;
};

class Triangle {
	ID3D11Buffer* vertexBuffer;
	Vertex vertices[3];
public:
	void init(ID3D11Device*& device) {
		vertices[0].position = Vec3(0, 1.0f, 0);
		vertices[0].color = Color(0, 1.0f, 0);
		vertices[1].position = Vec3(-1.0f, -1.0f, 0);
		vertices[1].color = Color(1.0f, 0, 0);
		vertices[2].position = Vec3(1.0f, -1.0f, 0);
		vertices[2].color = Color(0, 0, 1.0f);
		
	}

	// Create a buffer in GPU top store vertex
	void createBuffer(ID3D11Device* &device) {
		D3D11_BUFFER_DESC bd;
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;
		D3D11_SUBRESOURCE_DATA uploadData;
		bd.ByteWidth = sizeof(Vertex) * 3;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		uploadData.pSysMem = vertices;
		uploadData.SysMemPitch = 0;
		uploadData.SysMemSlicePitch = 0;
		device->CreateBuffer(&bd, &uploadData, &vertexBuffer);
	}
	
	// Draw triangle
	void draw(ID3D11DeviceContext* &devicecontext) {
		UINT offsets;
		offsets = 0;
		UINT strides = sizeof(Vertex);
		devicecontext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		devicecontext->IASetVertexBuffers(0, 1, &vertexBuffer, &strides, &offsets);
		devicecontext->Draw(3, 0);

	}
};