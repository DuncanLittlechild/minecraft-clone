#ifndef DL_SDLGPU_CAMERA3D_H
#define DL_SDLGPU_CAMERA3D_H
#include "core/Matrix4x4.h"
struct Camera3d{
	float zRot {};

	double fov {60.0 * SDL_PI_D/180.0};
	double aspectRatio{};

	double near {0.1};
	double far {100.0};

	Matrix4x4 viewMat {};
	Matrix4x4 projectionMat{};
	Matrix4x4 vpMat {};

	bool moveMade{false};

	void Init(double w, double h)
	{
		aspectRatio = (double)w/h;
//		UpdateAspectRatio(w,h);

	}

	void UpdateProjMat()
	{
		projectionMat = Matrix4x4_CreateProjectionMatrix(fov, aspectRatio, near, far);
	}
	void UpdateAspectRatio(double w, double h)
	{
		aspectRatio = (double)w/h;
		UpdateProjMat();
		UpdateVpMat();
	}
	void UpdateVpMat()
	{
		vpMat = Matrix4x4_Multiply(viewMat, projectionMat);
	}
};

#endif