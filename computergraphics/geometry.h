#pragma once
#include <d3d11.h>
#include "DXcore.h"
#include "mathLibrary.h"

struct STATIC_VERTEX
{
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
		memset(&data, 0, sizeof(D3D11_SUBRESOURCE_DATA));
		data.pSysMem = indices;
		device->CreateBuffer(&bd, &data, &indexBuffer);
		bd.ByteWidth = vertexSizeInBytes * numVertices;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		data.pSysMem = vertices;
		device->CreateBuffer(&bd, &data, &vertexBuffer);
		indicesSize = numIndices;
		strides = vertexSizeInBytes;
	}

	void init(vector<STATIC_VERTEX> vertices, vector<unsigned int> indices, ID3D11Device* device)
	{
		init(&vertices[0], sizeof(STATIC_VERTEX), vertices.size(), &indices[0], indices.size(), device);
	}

	void draw(ID3D11DeviceContext* devicecontext) {
		UINT offsets = 0;
		devicecontext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		devicecontext->IASetVertexBuffers(0, 1, &vertexBuffer, &strides, &offsets);
		devicecontext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
		devicecontext->DrawIndexed(indicesSize, 0, 0);

	}
};

class Plane {
public:
	Mesh plane;
	STATIC_VERTEX addVertex(Vec3 p, Vec3 n, float tu, float tv)
	{
		STATIC_VERTEX v;
		v.pos = p;
		v.normal = n;
		//Frame frame;
		//frame.fromVector(n);
		v.tangent = Vec3(0,0,0); // For now
		v.tu = tu;
		v.tv = tv;
		return v;
	}

	// Initialize the plane
	void init(DXcore* dx) {
		vector<STATIC_VERTEX> vertices;

		// Define vertices for the plane
		vertices.push_back(addVertex(Vec3(-15, 0, -15), Vec3(0, 1, 0), 0, 0));
		vertices.push_back(addVertex(Vec3(15, 0, -15), Vec3(0, 1, 0), 1, 0));
		vertices.push_back(addVertex(Vec3(-15, 0, 15), Vec3(0, 1, 0), 0, 1));
		vertices.push_back(addVertex(Vec3(15, 0, 15), Vec3(0, 1, 0), 1, 1));

		vector<unsigned int> indices;

		// Define indices for two triangles forming a rectangle
		indices.push_back(2); indices.push_back(1); indices.push_back(0);
		indices.push_back(1); indices.push_back(2); indices.push_back(3);

		// Use the Mesh class to initialize vertex and index buffers
		plane.init(vertices, indices, dx->device);
	}

	

	void draw(ID3D11DeviceContext* devicecontext) {
		plane.draw(devicecontext);
	}
};
  

//class Model {
//public:
//	vector<Mesh> tree;
//
//	GEMLoader::GEMModelLoader loader;
//	vector<GEMLoader::GEMMesh> gemmeshes;
//	loader.load(filename, gemmeshes);
//	for (int i = 0; i < gemmeshes.size(); i++) {
//		Mesh mesh;
//		vector<STATIC_VERTEX> vertices;
//		for (int j = 0; j < gemmeshes[i].verticesStatic.size(); j++) {
//			STATIC_VERTEX v;
//			memcpy(&v, &gemmeshes[i].verticesStatic[j], sizeof(STATIC_VERTEX));
//			vertices.push_back(v);
//		}
//		mesh.init(core, vertices, gemmeshes[i].indices);
//		meshes.push_back(mesh);
//	}
//
//};