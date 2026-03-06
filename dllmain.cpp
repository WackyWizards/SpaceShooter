#include "pch.h"
#include "Game.h"
#include <iostream>
#include <algorithm>

EngineBindings g_engine;

class SpaceShooter : public Game
{
public:
	void Init(const EngineBindings& bindings) override
	{
		g_engine = bindings;
		ResetEnemies();
	}

	void Update() override
	{
		MovePlayer();
		ShootBullet();
		MoveEnemies();
		CheckCollisions();

		if (AllEnemiesDead())
		{
			ResetEnemies();
		}

		BuildScene();
	}

	void Shutdown() override
	{
		std::printf("Final score: %d\n", score);
	}

private:
	float px = 0;
	float py = 0.7f;
	float bx = 0;
	float by = 0;
	float shootCooldown = 0;
	bool bulletAlive = false;

	void MovePlayer()
	{
		if (Input::KeyHeld(Key::Left) || Input::KeyHeld(Key::A))
		{
			px -= 1.2f * Time::Delta();
		}

		if (Input::KeyHeld(Key::Right) || Input::KeyHeld(Key::D))
		{
			px += 1.2f * Time::Delta();
		}

		if (px < -0.9f)
		{
			px = -0.9f;
		}

		if (px > 0.9f)
		{
			px = 0.9f;
		}
	}

	void ShootBullet()
	{
		shootCooldown -= Time::Delta();

		if (!bulletAlive && shootCooldown <= 0 && Input::KeyPressed(Key::Space))
		{
			bulletAlive = true;
			bx = px; by = py - 0.12f;
			shootCooldown = 0.3f;
		}

		if (bulletAlive)
		{
			by -= 2.0f * Time::Delta();
			if (by < -1.1f)
			{
				bulletAlive = false;
			}
		}
	}

	struct Enemy
	{
		float x = 0;
		float y = 0;
		float dir = 0;
		bool alive = false;
	};

	Enemy enemies[3]{};

	void ResetEnemies()
	{
		for (int i = 0; i < 3; i++)
		{
			enemies[i] = { -0.6f + i * 0.6f, -0.5f, (i % 2 == 0 ? 1.f : -1.f), true };
		}
	}

	void MoveEnemies()
	{
		for (auto& e : enemies)
		{
			if (!e.alive)
			{
				continue;
			}

			e.x += e.dir * 0.4f * Time::Delta();

			if (e.x > 0.85f)
			{
				e.x = 0.85f;
				e.dir = -1.f;
			}

			if (e.x < -0.85f)
			{
				e.x = -0.85f;
				e.dir = 1.f;
			}
		}
	}

	[[nodiscard]]
	bool AllEnemiesDead() const
	{
		for (const auto& e : enemies)
		{
			if (e.alive)
			{
				return false;
			}
		}

		return true;
	}

	int score = 0;

	static bool Overlaps(const float ax, const float ay, const float ahw, const float ahh, const float bx, const float by, const float bhw, const float bhh)
	{
		return std::abs(ax - bx) < ahw + bhw && std::abs(ay - by) < ahh + bhh;
	}

	void CheckCollisions()
	{
		if (!bulletAlive)
		{
			return;
		}

		for (auto& e : enemies)
		{
			if (!e.alive)
			{
				continue;
			}

			if (Overlaps(bx, by, 0.02f, 0.05f, e.x, e.y, 0.07f, 0.07f))
			{
				e.alive = false;
				bulletAlive = false;
				std::printf("Hit! Score: %d\n", ++score);
				return;
			}
		}
	}

	void BuildScene()
	{
		Scene::Push({.x = px, .y = py, .halfW = 0.06f, .halfH = 0.05f, .r = 0.2f, .g = 1.0f, .b = 0.3f });

		for (const auto& e : enemies)
		{
			if (e.alive)
			{
				Scene::Push({.x = e.x, .y = e.y, .halfW = 0.07f, .halfH = 0.07f, .r = 1.0f, .g = 0.2f, .b = 0.2f });
			}
		}

		if (bulletAlive)
		{
			Scene::Push({.x = bx, .y = by, .halfW = 0.02f, .halfH = 0.05f, .r = 1.0f, .g = 1.0f, .b = 0.1f });
		}
	}
};

extern "C" __declspec(dllexport) Game* CreateGame()
{
	return new SpaceShooter();
}