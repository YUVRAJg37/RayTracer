#include "Renderer.h"

void Renderer::onResize(uint32_t width, uint32_t height)
{
	if (m_FinalImage)
	{
		if (m_FinalImage->GetWidth() == width || m_FinalImage->GetHeight() == height)
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

void Renderer::Render()
{
	for (uint32_t y = 0; y < m_FinalImage->GetHeight(); y++)
	{
		for (uint32_t x = 0; x < m_FinalImage->GetWidth(); x++)
		{
			glm::vec2 coord{ (float)x / (float)m_FinalImage->GetWidth(), (float)y/(float)m_FinalImage->GetHeight() };
			coord = coord * 2.0f - 1.0f;
			m_ImageData[x + y*m_FinalImage->GetWidth()] = PerPixel(coord);
		}
	}

	m_FinalImage->SetData(m_ImageData);
}

uint32_t Renderer::PerPixel(glm::vec2 coord)
{
	uint8_t r = (uint8_t)(coord.x * 255.0f);
	uint8_t g = (uint8_t)(coord.y * 255.0f);

	// +ve Z = backward, -ve Z = forward

	glm::vec3 RayOrigin{ 0.0f, 0.0f, 2.0f };
	glm::vec3 RayDirection{ coord.x, coord.y, -1 };
	float radius = 0.5f;
	// (bx^2 + by^2 + bz^2)t^2 + 2(axbx + ayby + azbz)t + (ax^2 + ay^2 + az^2 - r^)
	// a = ray origin
	// b = ray direction
	// t = distnace
	// r = radius

	// Ax^2 + Bx + C

	float A = glm::dot(RayDirection, RayDirection);
	float B = glm::dot(RayOrigin, RayDirection) * 2;
	float C = glm::dot(RayOrigin, RayOrigin) - radius*radius;

	float discriminant = B * B - 4.0f * (A * C);

	if (discriminant >= 0)
		return 0xffff0000;

	return 0xff000000;

	//return 0xff000000 | (g << 8) | r ;
}