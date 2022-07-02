#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "Walnut/Image.h"
#include "Walnut/Random.h"
#include "Walnut/Timer.h"

using namespace Walnut;

class ExampleLayer : public Walnut::Layer
{
public:
	virtual void OnUIRender() override
	{
		ImGui::Begin("Settings");
		ImGui::Text("Render Time %.3fms", m_ElapsedTime);
		ImGui::Text("Circle Render Time %.4fms", c_ElapsedTime);
		if(ImGui::Button("Render"))
		{
			Render();
		}

		if (ImGui::Button("Render Circle"))
		{
			RenderCircle();
		}
		ImGui::End();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("ViewPort");

		m_ViewportHeight = ImGui::GetContentRegionAvail().y;
		m_ViewportWidth = ImGui::GetContentRegionAvail().x;


		if(m_Image)
			ImGui::Image(m_Image->GetDescriptorSet(), { (float)m_Image->GetWidth(), (float)m_Image->GetHeight() });

		
		ImGui::End();
		

		ImGui::Begin("Test");

		c_height = ImGui::GetContentRegionAvail().y;
		c_width = ImGui::GetContentRegionAvail().x;

		if (c_Image)
			ImGui::Image(c_Image->GetDescriptorSet(), { (float)c_Image->GetWidth(), (float)c_Image->GetHeight()});

		ImGui::End();
		ImGui::PopStyleVar();

	}

	void Render()
	{
		Timer timer;

		if (!m_Image || m_ViewportHeight != m_Image->GetHeight() || m_ViewportWidth != m_Image->GetWidth())
		{
			m_Image = std::make_shared<Image>(m_ViewportWidth, m_ViewportHeight, ImageFormat::RGBA);
			delete[] m_ImageData;

			m_ImageData = new uint32_t[m_ViewportHeight * m_ViewportWidth];
		}

		for (uint32_t i = 0; i < m_ViewportHeight * m_ViewportWidth; i++)
		{
			m_ImageData[i] = Random::UInt();
			m_ImageData[i] |= 0xff000000;
		}

		m_Image->SetData(m_ImageData);

		m_ElapsedTime = timer.ElapsedMillis();
	}

	void RenderCircle()
	{
		Timer timer;

		if (!c_Image || c_width != c_Image->GetWidth() || c_height != c_Image->GetWidth())
		{
			c_Image = std::make_shared<Image>(c_width, c_height, ImageFormat::RGBA);
			delete[] c_ImageData;

			c_ImageData = new uint32_t[c_width * c_height];
		}

		c_centerX = (float)c_width / 2;
		c_centerY = (float)c_height / 2;

		for (uint32_t i = 0; i < c_width * c_height; i++)
		{
			uint32_t c_eqn = std::pow(((i % c_width) - c_centerX), 2) + std::pow(((i / c_width) - c_centerY), 2);

			if (c_eqn <= c_radius * c_radius)
			{
				c_ImageData[i] = Random::UInt();
				c_ImageData[i] |= 0xff000000;
			}
			else
				c_ImageData[i] = 0xff000000;
		}

		c_Image->SetData(c_ImageData);

		c_ElapsedTime = timer.ElapsedMillis();
	}

private : 

	std::shared_ptr<Image> m_Image;
	uint32_t m_ViewportHeight{0}, m_ViewportWidth{0};
	uint32_t* m_ImageData = nullptr;

	std::shared_ptr<Image> c_Image;
	uint32_t c_height{ 0 }, c_width{ 0 }, c_radius{ 300 };
	float c_centerX{ 0 }, c_centerY{ 0 };
	uint32_t* c_ImageData;

	float m_ElapsedTime{ 0 };
	float c_ElapsedTime{ 0 };
};

Walnut::Application* Walnut::CreateApplication(int argc, char** argv)
{
	Walnut::ApplicationSpecification spec;
	spec.Name = "Ray Tracer";

	Walnut::Application* app = new Walnut::Application(spec);
	app->PushLayer<ExampleLayer>();
	app->SetMenubarCallback([app]()
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Exit"))
			{
				app->Close();
			}
			ImGui::EndMenu();
		}
	});
	return app;
}