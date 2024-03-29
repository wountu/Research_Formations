#include "SDLUtil.h"

SDLUtil::SDLUtil(const char* title, const int width, const int height)
	:m_Exit{}
	,m_TargetPos{}
	,m_GrabRect{}
	,m_Grabbing{}
	,m_StartGrabPos{}
	,m_ActiveGrabPos{}
	,m_WindowDimensions{float(width), float(height)}
	, m_RenderGraph{false}
{
	m_pWindow = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, 0);
	m_pRenderer = SDL_CreateRenderer(m_pWindow, -1, 0);
	SDL_SetRenderDrawColor(m_pRenderer, 255, 0, 0, 255);
}

SDLUtil::~SDLUtil()
{
	SDL_DestroyWindow(m_pWindow);
	SDL_DestroyRenderer(m_pRenderer);
	SDL_Quit();
}

void SDLUtil::Event()
{
	SDL_Event event{};
	SDL_PollEvent(&event);
	
	if (!m_Grabbing)
	{
		m_ActiveGrabPos.x = 0;
		m_ActiveGrabPos.y = 0;
		m_StartGrabPos.x = 0;
		m_StartGrabPos.y = 0;
	}

	m_ChangeTileCost = false;
	m_ToggleFormation = false;

	switch (event.type)
	{
	case SDL_QUIT:
		m_Exit = true;
		break;
	case SDL_MOUSEBUTTONDOWN:
		if (SDL_BUTTON_LEFT == event.button.button)
		{
			int x, y;
			SDL_GetMouseState(&x, &y);
			m_TargetPos.x = float(x);
			m_TargetPos.y = float(y);
		}

		if(SDL_BUTTON_RIGHT == event.button.button)
		{
			int x, y;
			SDL_GetMouseState(&x, &y);

			if (!m_Grabbing)
			{
				m_StartGrabPos.x = float(x);
				m_StartGrabPos.y = float(y);
				m_Grabbing = true;
			}

		}

		if (SDL_BUTTON_MIDDLE == event.button.button)
		{
			int x, y;
			SDL_GetMouseState(&x, &y);

			m_ChangeTileCost = true;
			m_TileSelectPos.x = static_cast<float>(x);
			m_TileSelectPos.y = static_cast<float>(y);
		}
		break;
	case SDL_MOUSEBUTTONUP:
		if (SDL_BUTTON_RIGHT == event.button.button)
		{
			//int x, y;
			//SDL_GetMouseState(&x, &y);
			//m_GrabRect.startPos = m_MousePos;
			//m_GrabRect.width = x - m_MousePos.x;
			//m_GrabRect.height = y - m_MousePos.y;

			std::cout << "stop grabbing" << "\n";

			m_Grabbing = false;
		}
		break;
	case SDL_KEYDOWN:
		if(event.key.keysym.scancode == SDL_SCANCODE_F1)
		{
			m_RenderGraph = !m_RenderGraph;
		}
		if (event.key.keysym.scancode == SDL_SCANCODE_F2)
		{
			m_ToggleFormation = true;
		}
		break;
	}

	if (m_Grabbing)
	{
		int x, y;
		SDL_GetMouseState(&x, &y);

		m_ActiveGrabPos.x = float(x);
		m_ActiveGrabPos.y = float(y);
	}
}

void SDLUtil::Update()
{
	float width{ m_ActiveGrabPos.x - m_StartGrabPos.x };
	float height{ m_ActiveGrabPos.y - m_StartGrabPos.y };
	m_GrabRect.startPos = m_StartGrabPos;
	m_GrabRect.width = width;
	m_GrabRect.height = height;
}

void SDLUtil::RenderBackground() const
{
	SDL_SetRenderDrawColor(m_pRenderer, 177, 177, 177, 255);
	SDL_RenderClear(m_pRenderer);  //Back color
}

void SDLUtil::Render() const
{
	SDLUtil::DrawRect(m_GrabRect);
	SDL_RenderPresent(m_pRenderer);
}

void SDLUtil::DrawCircle(Utils::Vector2 centre, float radius) const
{
	const float diameter{ radius * 2.f };

	float x{ radius - 1 };
	float y{ 0.f };
	float tx{ 1.f };
	float ty{ 1.f };
	float error{ tx - diameter };

	while (x >= y)
	{
		SDL_RenderDrawPoint(m_pRenderer, int(centre.x + x), int(centre.y - y));
		SDL_RenderDrawPoint(m_pRenderer, int(centre.x + x), int(centre.y + y));
		SDL_RenderDrawPoint(m_pRenderer, int(centre.x - x), int(centre.y - y));
		SDL_RenderDrawPoint(m_pRenderer, int(centre.x - x), int(centre.y + y));
		SDL_RenderDrawPoint(m_pRenderer, int(centre.x + y), int(centre.y - x));
		SDL_RenderDrawPoint(m_pRenderer, int(centre.x + y), int(centre.y + x));
		SDL_RenderDrawPoint(m_pRenderer, int(centre.x - y), int(centre.y - x));
		SDL_RenderDrawPoint(m_pRenderer, int(centre.x - y), int(centre.y + x));

		if (error <= 0)
		{
			++y;
			error += ty;
			ty += 2.f;
		}

		if (error > 0)
		{
			--x;
			tx += 2.f;
			error += tx - diameter;
		}
	}
};

void SDLUtil::DrawRect(Utils::Rect rect) const
{
	//auto SDLRect = new SDL_Rect{};
	//SDLRect->x = static_cast<float>(rect.startPos.x);
	//SDLRect->y = static_cast<float>(rect.startPos.y);
	//SDLRect->w = static_cast<float>(rect.width);
	//SDLRect->h = static_cast<float>(rect.height);

	//SDL_RenderFillRect(m_pRenderer, SDLRect);

	SDL_RenderDrawLine(m_pRenderer, int(rect.startPos.x), int(rect.startPos.y), int(rect.startPos.x), int(rect.startPos.y + rect.height));
	SDL_RenderDrawLine(m_pRenderer, int(rect.startPos.x), int(rect.startPos.y), int(rect.startPos.x + rect.width), int(rect.startPos.y));
	SDL_RenderDrawLine(m_pRenderer, int(rect.startPos.x + rect.width), int(rect.startPos.y), int(rect.startPos.x + rect.width), int(rect.startPos.y + rect.height));
	SDL_RenderDrawLine(m_pRenderer, int(rect.startPos.x), int(rect.startPos.y + rect.height), int(rect.startPos.x + rect.width), int(rect.startPos.y + rect.height));
}

void SDLUtil::DrawLine(Utils::Vector2 point1, Utils::Vector2 point2) const
{
	SDL_RenderDrawLine(m_pRenderer, int(point1.x), int(point1.y), int(point2.x), int(point2.y));
}

Utils::Vector2 SDLUtil::GetMousePos() const
{
	return m_TargetPos;
}

Utils::Rect SDLUtil::GetGrabRect() const
{
	return m_GrabRect;
}

Utils::Vector2 SDLUtil::GetWindowDimensions() const
{
	return m_WindowDimensions;
}

void SDLUtil::ChangeColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
	SDL_SetRenderDrawColor(m_pRenderer, r, g, b, a);
}

SDL_Renderer* SDLUtil::GetRenderer() const
{
	return m_pRenderer;
}

bool SDLUtil::GetRenderGraph() const
{
	return m_RenderGraph;
}

bool SDLUtil::ToggleFormation()
{
	return m_ToggleFormation;
}

bool SDLUtil::ChangeTile(Utils::Vector2 &pos)
{
	if (m_ChangeTileCost)
	{
		pos = m_TileSelectPos;
		return true;
	}
	else return false;	
}