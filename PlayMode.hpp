#include "Mode.hpp"

#include "Connection.hpp"
#include "Scene.hpp"

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
	glm::uvec2 drawable_size;
	glm::mat4 projection = glm::ortho(0.f, 800.f, 600.f, 0.f, -100.f, 100.f);

	Scene scene;

	//----- game state -----

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
	} left, right, down, up;

	//last message from server:
	std::string server_message;

	//connection to server:
	Client &client;

};
