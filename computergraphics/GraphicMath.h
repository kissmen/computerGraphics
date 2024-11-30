#pragma once
#define _USE_MATH_DEFINES // Enable common meth definitions from cmath, like M_PI
#include<cmath>
#define SQ(x) (x) * (x)
#define max(a,b) (a>b? a : b) // Finf maxnumber between a and b
#define min(a,b) (a>b? b : a)
#include"memory.h" // enable some memory related operations
#include"GamesEngineeringBase.h"
class Vec3 {
public:
	union {
		struct {
			float x;
			float y;
			float z;
		};
		float coords[3];
	};

	Vec3() { x = 0; y = 0; z = 0; }
	Vec3(float _x, float _y, float _z) {
		x = _x;
		y = _y;
		z = _z;
	}

	// Define basic calculations: addition, subtraction, multiplication with another vector
	// instead of functios, we use symbol override to complete.
	Vec3 operator+ (const Vec3& v) const {
		return Vec3(x + v.x, y + v.y, z + v.z);
	}
	Vec3 operator- (const Vec3& v) const{
		return Vec3(x - v.x, y - v.y, z - v.z);
	}
	Vec3 operator* (const Vec3& v) const {
		return Vec3(x * v.x, y * v.y, z * v.z);
	}
	Vec3 operator/ (const Vec3& v) const {
		if (v.x != 0 && v.y != 0 && v.z!= 0)
		return Vec3(x / v.x, y / v.y, z / v.z);
	}
	Vec3& operator+= (const Vec3& v) {
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}
	Vec3& operator-= (const Vec3 v) {
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return *this;
	}
	Vec3& operator*= (const Vec3 v) {
		x *= v.x;
		y *= v.y;
		z *= v.z;
		return *this;
	}
	Vec3& operator/= (const Vec3 v) {
		if (v.x != 0 && v.y != 0 && v.z != 0) {
			x /= v.x;
			y /= v.y;
			z /= v.z;
			return *this;
		}
	}
	// basic operations with another float number
	Vec3 operator+ (const float v) {
		return Vec3(x + v, y + v, z + v);
	}
	Vec3 operator- (const float v) {
		return Vec3(x - v, y - v, z - v);
	}
	Vec3 operator* (const float v) {
		return Vec3(x * v, y * v, z * v);
	}
	Vec3 operator/ (const float v) {
		return Vec3(x / v, y / v, z / v);
	}

	Vec3 operator+= (const float v) {
		return Vec3(x += v, y += v, z += v);
	}
	Vec3 operator-= (const float v) {

		return Vec3(x -= v, y -= v, z - v);
	}
	Vec3 operator*= (const float v) {
		return Vec3(x *= v, y *= v, z *= v);
	}
	Vec3 operator/= (const float v) {
		return Vec3(x /= v, y /= v, z /= v);
	}

	float Length() {
		return sqrt(x * x + y * y + z * z);
	}
	Vec3 normalise() const {
		float len = 1 / sqrtf(coords[0] * coords[0]+ coords[1] * coords[1]+ coords[2] * coords[2]);
		if (len == 0) {
			// Return a default or unchanged vector if length is zero
			return *this;
		}
		return Vec3(x * len, y * len, z * len);
	}
	float normaliseLength() {
		float length = sqrtf(coords[0] * coords[0] + coords[1] * coords[1] + coords[2] * coords[2]);
		float len = 1.f / length;
		coords[0] *= len; coords[1] *= len; coords[2] *= len;
		return length;
	}
	

	float Dot(const Vec3 v) const {
		return float(x * v.x + y * v.y + z * v.z);
	}
	// outside of matrix
	float Dot(const Vec3 &v1, const Vec3 &v2) {
		return float(v1.x*v2.x +v1.y*v2.y+v1.z*v2.z);

	}

	// cross product
	Vec3 Cross(const Vec3& v) const {
		return Vec3(v.y * z - v.z * y,
					v.z * x - v.x * z,
					v.x * y - v.y * x);
	}	

	Vec3 Max(const Vec3& v1, const Vec3& v2) {
		return Vec3(max(v1.x, v2.x),
			max(v1.y, v2.y),
			max(v1.z, v1.z));
	}

	float Max() const {
		return max(x, max(y, z));
	}
};

class Vec2 {
public:
	union {
		struct {
			float x;
			float y;
		};
		float coords[2];
	};
	Vec2(){}
	Vec2(float _x, float _y) {
		x = _x;
		y = _y;
	}


	Vec2 operator+ (const Vec2& v) const {
		return Vec2(x + v.x, y + v.y);
	}
	Vec2 operator- (const Vec2& v) const {
		return Vec2(x - v.x, y - v.y);
	}
	Vec2 operator* (const Vec2& v) const {
		return Vec2(x * v.x, y * v.y);
	}
	Vec2 operator/ (const Vec2& v) const {
		if (v.x != 0 && v.y != 0)
			return Vec2(x / v.x, y / v.y);
	}

	Vec2 operator+ (const float v) {
		return Vec2(x + v, y + v);
	}
	Vec2 operator- (const float v) {
		return Vec2(x - v, y - v);
	}
	Vec2 operator* (const float v) {
		return Vec2(x * v, y * v);
	}
	Vec2 operator/ (const float v) {
		return Vec2(x / v, y / v);
	}



	float Cross(const Vec2& v) const {
		return(x * v.y - y * v.x);
	}



	float Area() {
		return (sqrt(x * x + y * y));
	}
};



class Vec4 {
public:
	union {
		struct {
			float x;
			float y;
			float z;
			float w; // (0,1), represent vector(0), and point(1)
		};
		float coords[4];
	};
	Vec4() {};
	Vec4(float _x, float _y, float _z, float _w) {
		x = _x;
		y = _y;
		z = _z;
		w = _w;
	}


	Vec2 screenTrans(GamesEngineeringBase::Window& canvas) {
		return Vec2(((x + 1) / 2) * canvas.getWidth(), ((y + 1) / 2) * canvas.getHeight());
	}


	Vec2 screenTransNegative(GamesEngineeringBase::Window& canvas) {
		float screenX = ((x + 1) / 2) * canvas.getWidth();
		float screenY = ((1 - y) / 2) * canvas.getHeight(); // Flip y-axis if top-left origin
		screenX = max(0.0f, min(screenX, (float)canvas.getWidth()));

		// Clamp screenY to be within the valid screen range (if needed)
		screenY = max(0.0f, min(screenY, (float)canvas.getHeight()));

		return Vec2(screenX, screenY);
	}

};

class homoVec : public Vec4 {
public:
	homoVec(float _x, float _y, float _z, float _w) {
		// initialise vector to homogenous vector
		x = _x / _w;
		y = _y / _w;
		z = _z / _w;
		w = _w / _w;
	}
	homoVec(Vec4 v) {
		// initialise vector to homogenous vector
		if (v.w != 0) {
			x = v.x / v.w;
			y = v.y / v.w;
			z = v.z / v.w;
			w = 1;
		}
	}

	homoVec(Vec3& v) {
		// initialise vector to homogenous vector
		x = v.x;
		y = v.y;
		z = v.z;
		w = 1;
	}

	void SetHomo() {
		// initialise vector to homogenous vector
		if (w != 0) {
			x = x / w;
			y = y / w;
			z = z / w;
			w = w / w;
		}
	}

	void Normalise() {
		
	}

};

class Matrix {
public:
	union {
		float a[4][4];
		float m[16];
	};

	Matrix() {
		// Initialise to identity
		m[0] = 1;
		m[1] = 0;
		m[2] = 0;
		m[3] = 0;
		m[4] = 0;
		m[5] = 1;
		m[6] = 0;
		m[7] = 0;
		m[8] = 0;
		m[9] = 0;
		m[10] = 1;
		m[11] = 0;
		m[12] = 0;
		m[13] = 0;
		m[14] = 0;
		m[15] = 1;
	}

	Matrix(float* otherMatrix) {
		memcpy(m, otherMatrix, 16 * sizeof(float)); // copy the momory of othermatrix to m.
	}


	Matrix(float m00, float m01, float m02, float m03, float m10, float m11, float m12, float m13, float m20, float m21, float m22, float m23, float m30, float m31, float m32, float m33) {
		a[0][0] = m00;
		a[0][1] = m01;
		a[0][2] = m02;
		a[0][3] = m03;

		a[1][0] = m10;
		a[1][1] = m11;
		a[1][2] = m12;
		a[1][3] = m13;

		a[2][0] = m20;
		a[2][1] = m21;
		a[2][2] = m22;
		a[2][3] = m23;

		a[3][0] = m00;
		a[3][1] = m31;
		a[3][2] = m32;
		a[3][3] = m33;
	}


	float& operator[](int index) {
		return m[index];
	}
	Matrix& operator+(Matrix m1) {
		m[0] = m[0] + m1[0];
		m[1] = m[1] + m1[1];
		m[2] = m[2] + m1[2];
		m[3] = m[3] + m1[3];
		m[4] = m[4] + m1[4];
		m[5] = m[5] + m1[5];
		m[6] = m[6] + m1[6];
		m[7] = m[7] + m1[7];
		m[8] = m[8] + m1[8];
		m[9] = m[9] + m1[9];
		m[10] = m[10] + m1[10];
		m[11] = m[11] + m1[11];
		m[12] = m[12] + m1[12];
		m[13] = m[13] + m1[13];
		m[14] = m[14] + m1[14];
		m[15] = m[15] + m1[15];
		return *this;
	}

	Matrix& operator-(Matrix m1) {
		m[0] = m[0] - m1[0];
		m[1] = m[1] - m1[1];
		m[2] = m[2] - m1[2];
		m[3] = m[3] - m1[3];
		m[4] = m[4] - m1[4];
		m[5] = m[5] - m1[5];
		m[6] = m[6] - m1[6];
		m[7] = m[7] - m1[7];
		m[8] = m[8] - m1[8];
		m[9] = m[9] - m1[9];
		m[10] = m[10] - m1[10];
		m[11] = m[11] - m1[11];
		m[12] = m[12] - m1[12];
		m[13] = m[13] - m1[13];
		m[14] = m[14] - m1[14];
		m[15] = m[15] - m1[15];
		return *this;
	}

	static Matrix translation(const Vec3& v) {
		Matrix mat;
		mat.a[0][3] = v.x;
		mat.a[1][3] = v.y;
		mat.a[2][3] = v.z;
		return mat;

	}

	static Matrix scaling(const Vec3& v) {
		Matrix mat;
		mat.m[0] = v.x;
		mat.m[5] = v.y;
		mat.m[10] = v.z;
		return mat;
	}

	static Matrix rotationX(const float theta) {
		Matrix mat;
		float cost = cosf(theta);
		float sint = sinf(theta);


		mat.m[5] = cost;
		mat.m[6] = -sint;
		mat.m[9] = sint;
		mat.m[10] = cost;
	}

	static Matrix rotationY(const float theta) {
		Matrix mat;
		float cost = cosf(theta);
		float sint = sinf(theta);

		mat.m[0] = cost;
		mat.m[2] = sint;
		mat.m[9] = -sint;
		mat.m[11] = cost;

	}

	static Matrix rotationZ(const float theta) {
		Matrix mat;
		float cost = cosf(theta);
		float sint = sinf(theta);

		mat.m[0] = cost;
		mat.m[1] = -sint;
		mat.m[4] = sint;
		mat.m[5] = cost;

	}


	Matrix mul(Matrix& m1) {
		Matrix temp;
		temp.m[0] = m[0] * m1[0] + m[1] * m1[4] + m[2] * m1[8] + m[3] * m1[12];
		temp.m[1] = m[0] * m1[1] + m[1] * m1[5] + m[2] * m1[9] + m[3] * m1[13];
		temp.m[2] = m[0] * m1[2] + m[1] * m1[6] + m[2] * m1[10] + m[3] * m1[14];
		temp.m[3] = m[0] * m1[3] + m[1] * m1[7] + m[2] * m1[11] + m[3] * m1[15];

		temp.m[4] = m[4] * m1[0] + m[5] * m1[4] + m[6] * m1[8] + m[7] * m1[12];
		temp.m[5] = m[4] * m1[1] + m[5] * m1[5] + m[6] * m1[9] + m[7] * m1[13];
		temp.m[6] = m[4] * m1[2] + m[5] * m1[6] + m[6] * m1[10] + m[7] * m1[14];
		temp.m[7] = m[4] * m1[3] + m[5] * m1[7] + m[6] * m1[11] + m[7] * m1[15];

		temp.m[8] = m[8] * m1[0] + m[9] * m1[4] + m[10] * m1[8] + m[11] * m1[12];
		temp.m[9] = m[8] * m1[1] + m[9] * m1[5] + m[10] * m1[9] + m[11] * m1[13];
		temp.m[10] = m[8] * m1[2] + m[9] * m1[6] + m[10] * m1[10] + m[11] * m1[14];
		temp.m[11] = m[8] * m1[3] + m[9] * m1[7] + m[10] * m1[11] + m[11] * m1[15];

		temp.m[12] = m[12] * m1[0] + m[13] * m1[4] + m[14] * m1[8] + m[15] * m1[12];
		temp.m[13] = m[12] * m1[1] + m[13] * m1[5] + m[14] * m1[9] + m[15] * m1[13];
		temp.m[14] = m[12] * m1[2] + m[13] * m1[6] + m[14] * m1[10] + m[15] * m1[14];
		temp.m[15] = m[12] * m1[3] + m[13] * m1[7] + m[14] * m1[11] + m[15] * m1[15];
		return temp;
	}

	Matrix operator*(Matrix& m1)  {
		return mul(m1);
	}

	Vec4 operator*(const Vec4& v) const {
		return Vec4(m[0] * v.x + m[1] * v.y + m[2] * v.z + m[3] * v.w,
			m[4] * v.x + m[5] * v.y + m[6] * v.z + m[7] * v.w,
			m[8] * v.x + m[9] * v.y + m[10] * v.z + m[11] * v.w,
			m[12] * v.x + m[13] * v.y + m[14] * v.z + m[15] * v.w);
	}

	homoVec operator*(const homoVec& v) const {
		return homoVec(m[0] * v.x + m[1] * v.y + m[2] * v.z + m[3] * v.w,
			m[4] * v.x + m[5] * v.y + m[6] * v.z + m[7] * v.w,
			m[8] * v.x + m[9] * v.y + m[10] * v.z + m[11] * v.w,
			m[12] * v.x + m[13] * v.y + m[14] * v.z + m[15] * v.w);
	}


	void identity(Matrix& a) {
		for (int i = 0; i < 16; ++i) {
			a[i] = (i % 5 == 0) ? 1 : 0;
		}
	}

	void identity() {
		for (int i = 0; i < 16; ++i) {
			m[i] = (i % 5 == 0) ? 1 : 0; 
		}
	}
	
	Matrix transpose(Matrix& matrix) {
		Matrix temp = matrix;
		matrix[1] = matrix[4];
		matrix[2] = matrix[9];
		matrix[3] = matrix[12];
		matrix[6] = matrix[9];
		matrix[7] = matrix[13];
		matrix[12] = matrix[15];

		matrix[4] = temp[1];
		matrix[9] = temp[2];
		matrix[12] = temp[3];
		matrix[9] = temp[6];
		matrix[13] = temp[7];
		matrix[15] = temp[12];
		return matrix;
	}


	Matrix invert()
	{
		Matrix inv;
		// cofactors
		inv[0] = m[5] * m[10] * m[15] + m[9] * m[14] * m[7] + m[6] * m[11] * m[13] - m[13] * m[10] * m[7] - m[6] * m[9] * m[15] - m[5] * m[11] * m[14];
		inv[1] = -(m[4] * m[10] * m[15] + m[7] * m[8] * m[14] + m[6] * m[11] * m[12] - m[12] * m[10] * m[7] - m[6] * m[8] * m[15] - m[4] * m[11] * m[14]);
		inv[2] = m[4] * m[9] * m[15] + m[7] * m[8] * m[13] + m[12] * m[5] * m[11] - m[12] * m[7] * m[9] - m[5] * m[8] * m[15] - m[4] * m[11] * m[13];
		inv[3] = -(m[4] * m[9] * m[14] + m[6] * m[8] * m[13] + m[5] * m[10] * m[12] - m[6] * m[9] * m[12] - m[5] * m[8] * m[14] - m[4] * m[10] * m[13]);

		inv[4] = -(m[1] * m[10] * m[15] + m[3] * m[9] * m[14] + m[2] * m[11] * m[13] - m[3] * m[10] * m[13] - m[2] * m[9] * m[15] - m[1] * m[11] * m[14]);
		inv[5] = m[0] * m[10] * m[15] + m[3] * m[8] * m[14] + m[2] * m[11] * m[12] - m[3] * m[10] * m[12] - m[2] * m[8] * m[15] - m[0] * m[11] * m[14];
		inv[6] = -(m[0] * m[9] * m[15] + m[3] * m[8] * m[13] + m[1] * m[11] * m[12] - m[3] * m[9] * m[12] - m[1] * m[8] * m[15] - m[0] * m[11] * m[13]);
		inv[7] = m[0] * m[9] * m[14] + m[2] * m[8] * m[13] + m[1] * m[10] * m[12] - m[2] * m[9] * m[12] - m[1] * m[8] * m[14] - m[0] * m[10] * m[13];

		inv[8] = m[1] * m[6] * m[15] + m[3] * m[5] * m[14] + m[2] * m[7] * m[13] - m[3] * m[6] * m[13] - m[2] * m[5] * m[15] - m[1] * m[7] * m[14];
		inv[9] = -(m[0] * m[6] * m[15] + m[3] * m[4] * m[14] + m[2] * m[7] * m[12] - m[3] * m[6] * m[12] - m[2] * m[4] * m[15] - m[0] * m[7] * m[14]);
		inv[10] = m[0] * m[5] * m[15] + m[1] * m[7] * m[12] + m[3] * m[4] * m[13] - m[3] * m[5] * m[12] - m[1] * m[4] * m[15] - m[0] * m[7] * m[13];
		inv[11] = -(m[0] * m[5] * m[14] + m[2] * m[4] * m[13] + m[1] * m[6] * m[12] - m[2] * m[5] * m[12] - m[1] * m[14] * m[4] - m[0] * m[6] * m[13]);

		inv[12] = -(m[1] * m[6] * m[11] + m[3] * m[5] * m[10] + m[2] * m[7] * m[9] - m[3] * m[6] * m[9] - m[2] * m[5] * m[11] - m[1] * m[7] * m[10]);
		inv[13] = m[0] * m[6] * m[11] + m[2] * m[7] * m[8] + m[3] * m[4] * m[10] - m[3] * m[6] * m[8] - m[2] * m[4] * m[11] - m[0] * m[7] * m[10];
		inv[14] = -(m[0] * m[5] * m[11] + m[3] * m[4] * m[9] + m[1] * m[7] * m[8] - m[3] * m[5] * m[8] - m[1] * m[4] * m[11] - m[0] * m[7] * m[9]);
		inv[15] = m[0] * m[5] * m[10] + m[1] * m[6] * m[8] + m[2] * m[4] * m[9] - m[2] * m[5] * m[8] - m[1] * m[4] * m[10] - m[0] * m[6] * m[9];


		float det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];
		if (det == 0) {
			inv.identity(inv);
		}
		det = 1.0 / det;
		for (int i = 0; i < 16; i++) {
			inv[i] = inv[i] * det; // multiple each element with det
		}
		return inv;
	}

	Matrix PerspProj(GamesEngineeringBase::Window&canvas, float fov, float nearPlane, float farPlane) {
		// Create a perspective projection matrix for 3d points to 2d space
		float aspect = static_cast<float>(canvas.getWidth()) / static_cast<float>(canvas.getHeight());
		float tanHalfTheta = tanf(fov / 2);
		Matrix temp;
		temp[0] = 1 / (aspect * tanHalfTheta);
		temp[5] = 1 / tanHalfTheta;
		temp[10] = -farPlane / (farPlane - nearPlane);
		temp[11] = -(farPlane * nearPlane) / (farPlane - nearPlane);
		temp[14] = -1;
		temp[15] = 0;
		return temp;
	}

	Matrix PerspProj1(GamesEngineeringBase::Window& canvas, float fov, float nearPlane, float farPlane) {
		float aspect = static_cast<float>(canvas.getWidth()) / static_cast<float>(canvas.getHeight());
		float tanHalfTheta = tanf(fov / 2);
		Matrix temp;

		temp[0] = 1 / (aspect * tanHalfTheta);      // X-axis scaling
		temp[5] = 1 / tanHalfTheta;                // Y-axis scaling
		temp[10] = -(farPlane + nearPlane) / (farPlane - nearPlane); // Z-axis scaling
		temp[11] = -(2 * farPlane * nearPlane) / (farPlane - nearPlane); // Z-axis translation
		temp[14] = -1;                             // Perspective divide
		temp[15] = 0;                              // Homogeneous coordinate

		return temp;
	}

	Matrix screenTrans(GamesEngineeringBase::Window& canvas) {
		Matrix temp;
		temp.m[0] = canvas.getWidth() / 2;
		temp.m[3] = canvas.getWidth() / 2;
		temp.m[5] = -static_cast<float>(canvas.getHeight() / 2); // set origin to left bottom
		temp.m[7] = canvas.getHeight() / 2;
		return temp;
	}


	Matrix Lookat(Vec3& camera, Vec3& object, Vec3& upVec) {
		Matrix temp;
		Vec3 dir = (object - camera).normalise();

		Vec3 right = dir.Cross(upVec.normalise()).normalise();
		
		Vec3 up = right.Cross(dir);

		// FROM MEANS CAMERA OR ?
		temp[0] = right.x;
		temp[1] = right.y;
		temp[2] = right.z;
		temp[3] = -camera.Dot(right);

		temp[4] = up.x;
		temp[5] = up.y;
		temp[6] = up.z;
		temp[7] = -camera.Dot(up);

		temp[8] = dir.x;
		temp[9] = dir.y;
		temp[10] = dir.z;
		temp[11] = -camera.Dot(dir);

		temp[12] = 0;
		temp[13] = 0;
		temp[14] = 0;
		temp[15] = 1;
		return temp;
	}
};

class ShadingFrame {
public:
	Vec3 n;
	Vec3 t;
	Vec3 b;

	//Vec3 Construct(const Vec3& v) {
	//	Vec3 a = abs(v.x) > 0.9f ? Vec3(0, 1, 0) : Vec3(1, 0, 0);
	//	// compute t
	//	t = (a - v * v.Dot(a)).normalise();//????

	//	b = v.Cross(t);

	//	return *this;
	//}




	void Construct(const Vec3& v) {
		// Set n to the input vector and normalize it
		n = v.normalise(); // Assuming `v` is the normal

		// Choose an arbitrary vector for orthogonalization
		Vec3 a = abs(n.x) > 0.9f ? Vec3(0, 1, 0) : Vec3(1, 0, 0);

		// Compute t by orthogonalizing `a` with respect to `n`
		t = (a - n * n.Dot(a)).normalise();

		// Compute b as the cross product of n and t
		b = n.Cross(t);
	}

	Vec3 toWorld(const Vec3& v) {
		return t * v.x + b * v.y + v.z;
	}

	Vec3 toLocal(const Vec3& w) {
		return Vec3(w.Dot(t), w.Dot(b), w.Dot(n));
	}
};

class Colour {
public:
	union {
		struct {
			float r;
			float g;
			float b;
			float a;
		};
		float coords[4];
	};

	Colour(){}
	Colour(float _r, float _g, float _b, float _a) {
		r = _r;
		g = _g;
		b = _b;
		a = _a;
	}

	Colour TriangleColour(Vec3& v) const {
		// accept barycentric coordinates and return colours
		return Colour(v.x * 255, v.y * 255, v.z * 255, a);
	}
	Colour operator+(const Colour& colour) const {
		return Colour(r + colour.r, g + colour.g, b + colour.b, a + colour.a);
	}

	Colour operator*(const Colour& colour) const {
		return Colour(r * colour.r, g * colour.g, b * colour.b, a * colour.a);
		}
			
	Colour operator*(const float other) const {
		return Colour(r * other, g * other, b * other, a * other);
		}
	Colour operator/(const float other) const {
		return Colour(r / other, g / other, b / other, a / other);
		}
};
//
//class Triangle {
//	Vec4 ver1;
//	Vec4 ver2;
//	Vec4 ver3;
//	Triangle(Vec4 _ver1, Vec4 _ver2, Vec4 _ver3) {
//		ver1 = _ver1;
//		ver2 = _ver2;
//		ver3 = _ver3;
//	}
	// By the property of triangle

	//void Z_buffer() {
	//	// 
	//	zbuffer[i][j] = 1;
	//	int beginLength = min(ver1.x, min(ver2.x, ver3.x));
	//	int endLength = max(ver1.x, max(ver2.x, ver3.x));
	//	int beginHeight = min(ver1.y, min(ver2.y, ver3.y));
	//	int endHeight = max(ver1.y, max(ver2.y, ver3.y));
	//	for (unsigned int i = beginLength; i < endHeight; i++) {
	//		for (unsigned int j = beginHeight; j < endHeight; j++) {
	//			
	//		}
	//	}
	//}
//
//};

//
//float edge(Vec2 point1, Vec2 point2, Vec2 p) {
//	Vec2 q = p - point2;
//	Vec2 e = point1 - point2;
//	// std::cout << "cross product" << q.Cross(e) << std::endl;
//	return q.Cross(e);
//}
//
//Vec3 barycentric(Vec2 point1, Vec2 point2, Vec2 point3, Vec2 p) {
//	// return barycentric coordinates as 3d vector, p MUST inside the triangle generated by p1,p2,p3
//	Vec2 e0 = point1 - point2;
//	Vec2 e1 = point3 - point2;
//	// area of triangle
//	float area = abs(e0.Cross(e1)) / 2.f; // cross product is parallelogram with e0, e1 width and height, get the absolute value and divided by 2 is the area of triangle
//	// use area relation to get barycentric values for point p
//	float alpha = abs(edge(point3, point2, p)) / 2 / area;
//	float beta = abs(edge(point1, point3, p)) / 2 / area;
//	float gamma = abs(edge(point2, point1, p)) / 2 / area;
//	// return
//	if (alpha > 0 && beta > 0 && gamma > 0 && alpha + beta + gamma <= 1)
//		return Vec3(alpha, beta, gamma);
//}