#pragma once

#include "Mode.hpp"

#include "Connection.hpp"
#include "Scene.hpp"
#include "TileDrawer.hpp"
#include "Collider.hpp"

#include <glm/glm.hpp>

#include <vector>
#include <deque>

struct PlayMode : Mode {
	PlayMode(Client &client);
	virtual ~PlayMode();

	//functions called by main loop:
	virtual bool handle_event(SDL_Event const &, glm::uvec2 const &window_size) override;
	virtual void update(float elapsed) override;
	virtual void draw(glm::uvec2 const &drawable_size) override;

	// Game configurations
	glm::vec2 drawable_size;

	TileDrawer tile_drawer;

	Collider collider;

	const float horizontal_speed = 180.f;
	const float jump_velocity = 250.f;
	const float gravity_acc = 300.f;

	//----- game state -----

	// scene components
	std::vector<size_t> background;
	std::vector<size_t> map_components;
	size_t self_index;

	// player state
	struct PlayerState
	{
		glm::vec2 position;
		glm::vec2 velocity;
		// other states...
	};
	PlayerState player;
	PlayerState opponent;
	

	//input tracking:
	struct Button {
		uint8_t downs = 0;
		uint8_t pressed = 0;
	} left, right, down, up, jump;

	//last message from server:
	std::string server_message;

	//connection to server:
	Client &client;

};
