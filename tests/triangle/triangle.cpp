
#include <renderer.h>
#include <utils.h>
#include <vertices.h>

namespace Test
{
	class Concise::Renderer;
	
	class Triangle
	{
	private:
		Concise::Renderer * renderer;
	public:
		Triangle() 
		{
			renderer = new Concise::Renderer();
		}
		~Triangle() 
		{
			SAFE_DELETE(renderer);
		}
		
	public:
		void InitWindow(HINSTANCE hInstance, WNDPROC wndproc)
		{
			renderer->BuildWindow(hInstance, wndproc, "", "triangle");
		}
	
		void Init()
		{
			std::vector<Concise::Vertex> verticesData =
			{
				{ {  1.0f,  1.0f, 0.0f }, { 1.0f, 0.0f, 0.0f } },
				{ { -1.0f,  1.0f, 0.0f }, { 0.0f, 1.0f, 0.0f } },
				{ {  0.0f, -1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f } }
			};
			
			std::vector<Concise::UInt32> indicesData = { 0, 1, 2 };
			
			renderer->SubmitVerticesData(verticesData, indicesData);
		}
		
		void Loop()
		{
			renderer->Loop();
		}
	};
	
	// Windows entry point
	Triangle * triangle;
	LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		if (triangle != NULL)
		{
			triangle->handleMessages(hWnd, uMsg, wParam, lParam);
		}
		return (DefWindowProc(hWnd, uMsg, wParam, lParam));
	}
	
	int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow)
	{
		//for (size_t i = 0; i < __argc; i++) { VulkanExample::args.push_back(__argv[i]); };
		
		triangle = new Triangle();
		triangle->InitWindow(hInstance, WndProc);
		triangle->Init();
		triangle->Loop();
		
		delete(triangle);
		return 0;
	}

}