#pragma once

#include "Walnut/Image.h"
#include "Walnut/Random.h"
#include <memory>
#include "Ray.h"
#include "Camera.h"

class Renderer
{
public :

	Renderer() = default;

	void Render(const Camera &camera);
	void onResize(uint32_t width, uint32_t height);

	std::shared_ptr<Walnut::Image> GetFinalImage() const { return m_FinalImage; }

private:

	glm::vec4 TraceRay(const Ray& ray);

private:

	std::shared_ptr<Walnut::Image> m_FinalImage;
	uint32_t* m_ImageData = nullptr;
};