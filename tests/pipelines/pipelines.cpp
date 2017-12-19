


namespace Test
{
namespace Test
{
	class Concise::Renderer;
	
	class Pipelines
	{
	private:
		Concise::Renderer * m_renderer;
	public:
		Pipelines() 
		{
			m_renderer = new Concise::Renderer();
		}
		~Pipelines() 
		{
			SAFE_DELETE(m_renderer);
		}
		
	public:
		void InitWindow(HINSTANCE hInstance, WNDPROC wndproc)
		{
			m_renderer->BuildWindow(hInstance, wndproc, "abc", "triangle");
		}
	
		void Init()
		{
			m_renderer->Init();
			std::vector<Concise::Vertex> verticesData =
			{
				{ {  1.0f,  1.0f, 0.0f }, { 1.0f, 0.0f, 0.0f } },
				{ { -1.0f,  1.0f, 0.0f }, { 0.0f, 1.0f, 0.0f } },
				{ {  0.0f, -1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f } }
			};
			
			std::vector<Concise::UInt32> indicesData = { 0, 1, 2 };
			
			m_renderer->SubmitVerticesData(verticesData, indicesData);
			m_renderer->BuildCommandBuffers();
		}
		
		void Loop()
		{
			m_renderer->Loop();
		}

		void HandleMessages(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
		{
			if (!IsPrepared())
				return;

			m_renderer->HandleMessages(hWnd, uMsg, wParam, lParam);
		}

		bool IsPrepared() const { return m_renderer->IsPrepared(); }
	};
	

}
// Windows entry point
Test::Pipelines * triangle;
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
		

	if (triangle != NULL)
	{
		triangle->HandleMessages(hWnd, uMsg, wParam, lParam);
	}
	return (DefWindowProc(hWnd, uMsg, wParam, lParam));
}
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow)
{
	//for (size_t i = 0; i < __argc; i++) { VulkanExample::args.push_back(__argv[i]); };

	triangle = new Test::Pipelines();
	triangle->InitWindow(hInstance, WndProc);
	triangle->Init();
	triangle->Loop();

	delete(triangle);
	return 0;
}
	
}