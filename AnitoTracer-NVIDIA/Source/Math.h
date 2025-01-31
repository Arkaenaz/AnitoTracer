#pragma once

#include "_externals/glm/glm/vec3.hpp"
#include "_externals/glm/glm/mat4x4.hpp"

glm::mat4 SetToMatTranslation(glm::vec3 pos)
{
	glm::mat4 mat = glm::mat4(1.f);
	mat[3][0] = pos.x;
	mat[3][1] = pos.y;
	mat[3][2] = pos.z;

	return mat;
}

glm::mat4 SetToMatScale(glm::vec3 scale) {
	glm::mat4 mat = glm::mat4(1.f);
	mat[0][0] = scale.x;
	mat[1][1] = scale.y;
	mat[2][2] = scale.z;
	return mat;
}

glm::mat4 SetToMatRotationX(float x) {
	glm::mat4 mat = glm::mat4(1.f);
	mat[1][1] = cos(x);
	mat[1][2] = sin(x);
	mat[2][1] = -sin(x);
	mat[2][2] = cos(x);

	return mat;
}

glm::mat4 SetToMatRotationY(float y) {
	glm::mat4 mat = glm::mat4(1.f);
	mat[0][0] = cos(y);
	mat[0][2] = -sin(y);
	mat[2][0] = sin(y);
	mat[2][2] = cos(y);

	return mat;
}

glm::mat4 SetToMatRotationZ(float z) {
	glm::mat4 mat = glm::mat4(1.f);
	mat[0][0] = cos(z);
	mat[1][0] = -sin(z);
	mat[0][1] = sin(z);
	mat[1][1] = cos(z);

	return mat;
}

glm::vec3 GetPosFromMat(glm::mat4 mat) {
	return glm::vec3(mat[3][0], mat[3][1], mat[3][2]);
}

glm::vec3 GetScaleFromMat(glm::mat4 mat) {
	return glm::vec3(mat[0][0], mat[1][1], mat[2][2]);
}

glm::vec3 GetZDirFromMat(glm::mat4 mat) {
	return glm::vec3(mat[2][0], mat[2][1], mat[2][2]);
}

glm::vec3 GetYDirFromMat(glm::mat4 mat) {
	return glm::vec3(mat[1][0], mat[1][1], mat[1][2]);
}

glm::vec3 GetXDirFromMat(glm::mat4 mat) {
	return glm::vec3(mat[0][0], mat[0][1], mat[0][2]);
}

glm::vec4 Cross(glm::vec4& v1, glm::vec4& v2, glm::vec4& v3)
{
	glm::vec4 out;
	out.x = v1.y * (v2.z * v3.w - v3.z * v2.w) - v1.z * (v2.y * v3.w - v3.y * v2.w) + v1.w * (v2.y * v3.z - v2.z * v3.y);
	out.y = -(v1.x * (v2.z * v3.w - v3.z * v2.w) - v1.z * (v2.x * v3.w - v3.x * v2.w) + v1.w * (v2.x * v3.z - v3.x * v2.z));
	out.z = v1.x * (v2.y * v3.w - v3.y * v2.w) - v1.y * (v2.x * v3.w - v3.x * v2.w) + v1.w * (v2.x * v3.y - v3.x * v2.y);
	out.w = -(v1.x * (v2.y * v3.z - v3.y * v2.z) - v1.y * (v2.x * v3.z - v3.x * v2.z) + v1.z * (v2.x * v3.y - v3.x * v2.y));

	return out;
}

float GetDeterminant(glm::mat4 mat) {
	glm::vec4 minor, v1, v2, v3;
	float det;

	v1 = glm::vec4(mat[0][0], mat[1][0], mat[2][0], mat[3][0]);
	v2 = glm::vec4(mat[0][1], mat[1][1], mat[2][1], mat[3][1]);
	v3 = glm::vec4(mat[0][2], mat[1][2], mat[2][2], mat[3][2]);

	minor = Cross(v1, v2, v3);
	det = -(mat[0][3] * minor.x + mat[1][3] * minor.y + mat[2][3] * minor.z +
		mat[3][3] * minor.w);
	return det;
}

glm::mat4 Inverse(glm::mat4 mat) {
	int a, i, j;
	glm::mat4 out;
	glm::vec4 v, vec[3];
	float det = 0.0f;

	det = GetDeterminant(mat);
	if (det)
	{
		for (i = 0; i < 4; i++)
		{
			for (j = 0; j < 4; j++)
			{
				if (j != i)
				{
					a = j;
					if (j > i) a = a - 1;
					vec[a].x = (mat[j][0]);
					vec[a].y = (mat[j][1]);
					vec[a].z = (mat[j][2]);
					vec[a].w = (mat[j][3]);
				}
			}
			v = Cross(vec[0], vec[1], vec[2]);

			out[0][i] = pow(-1.0f, (float)i) * v.x / det;
			out[1][i] = pow(-1.0f, (float)i) * v.y / det;
			out[2][i] = pow(-1.0f, (float)i) * v.z / det;
			out[3][i] = pow(-1.0f, (float)i) * v.w / det;
		}
	}
	return out;
}