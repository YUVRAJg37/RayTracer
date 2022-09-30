#include "Renderer.h"
#include <iostream>

namespace utils
{
	//converting RGBA(vec4) to uint32 => (1, 0, 0, 1) = (1111 1111  0000 0000  0000 0000  1111 1111)
	static uint32_t ConvertToRGBA(glm::vec4& color)
	{
		uint8_t red = (uint8_t)(color.r * 255.0f);
		uint8_t green = (uint8_t)(color.g * 255.0f);
		uint8_t blue = (uint8_t)(color.b * 255.0f);
		uint8_t alpha = (uint8_t)(color.a * 255.0f);

		return (alpha << 24) | (blue << 16) | (green << 8) | red;
	}
}

void Renderer::onResize(uint32_t width, uint32_t height)
{
	
	if (m_FinalImage)
	{
		if (m_FinalImage->GetWidth() == width && m_FinalImage->GetHeight() == height)
			return;
		m_FinalImage->Resize(width, height);
	}
	else
	{
		m_FinalImage = std::make_shared<Walnut::Image>(width, height, Walnut::ImageFormat::RGBA);
	}

	delete[] m_ImageData;
	m_ImageData = new uint32_t[height * width];
}

void Renderer::Render(const Camera& camera)
{
	Ray ray;
	
	ray.RayOrigin = camera.GetPosition();;

	for (uint32_t y = 0; y < m_FinalImage->GetHeight(); y++)
	{
		for (uint32_t x = 0; x < m_FinalImage->GetWidth(); x++)
		{
			ray.RayDirection = camera.GetRayDirections()[x + y * m_FinalImage->GetWidth()];

			glm::vec4 color = TraceRay(ray);
			color = glm::clamp(color, glm::vec4(0.0f), glm::vec4(1.0f));

			m_ImageData[x + y*m_FinalImage->GetWidth()] = utils::ConvertToRGBA(color);
		}
	}

	m_FinalImage->SetData(m_ImageData);
}

glm::vec4 Renderer::TraceRay(const Ray& ray)
{
	// +ve Z = backward, -ve Z = forward
	float radius = 0.5f;
	// (bx^2 + by^2 + bz^2)t^2 + 2(axbx + ayby + azbz)t + (ax^2 + ay^2 + az^2 - r^)
	// a = ray origin
	// b = ray direction
	// t = distance
	// r = radius

	// Ax^2 + Bx + C

	float A = glm::dot(ray.RayDirection, ray.RayDirection);
	float B = glm::dot(ray.RayOrigin, ray.RayDirection) * 2;
	float C = glm::dot(ray.RayOrigin, ray.RayOrigin) - radius*radius;

	float discriminant = B * B - 4.0f * (A * C);

	if (discriminant < 0.0f)
		return glm::vec4(0, 0, 0, 1);

	//Solution = {-b+-sqrt(D)}/2a

	//Distance A+Bt
	float t0 = (- B + glm::sqrt(discriminant))/2.0f * A;
	float closestT = (- B - glm::sqrt(discriminant))/(2.0f * A);


	//Hit Points
	glm::vec3 h0 = ray.RayOrigin + ray.RayDirection * t0;
	glm::vec3 HitPoint = ray.RayOrigin + ray.RayDirection * closestT;
	glm::vec3 normal =  glm::normalize(HitPoint);
	glm::vec3 lightDirection = glm::normalize(glm::vec3(-1, -1, -1));

	float lightIntensity = glm::max(glm::dot(normal, -lightDirection), 0.0f);

	glm::vec3 SphereColor = glm::vec3(1, 0, 1);
	SphereColor*=lightIntensity;
	//SphereColor = SphereColor * 0.5f + 0.5f;

	return glm::vec4(SphereColor, 1.0f);
}