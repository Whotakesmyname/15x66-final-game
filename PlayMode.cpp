#include "PlayMode.hpp"

#include "DrawLines.hpp"
#include "gl_errors.hpp"
#include "data_path.hpp"
#include "hex_dump.hpp"
#include "DrawText.hpp"
#include "Mesh.hpp"
#include "Load.hpp"
#include "TileDrawer.hpp"

#include <glm/gtc/type_ptr.hpp>

#include <random>


PlayMode::PlayMode(Client &client_) : drawable_size({1280U, 720U}), client(client_) {
	// sample hardcoded map components

	size_t index;
	index = tile_drawer.add_component(TileDrawer::Square{
	glm::vec2(drawable_size.x / 2.f, drawable_size.y - 20), // position/center
	glm::vec2(drawable_size.x, 40), // size (x, y)
	glm::vec2(), // uv coord upper left
	glm::vec2() // uv coord bottom right
	}, TileDrawer::BACKGROUND); // rendering queue
	// convert component data to vertices and push to GPU
	tile_drawer.update_vertices(TileDrawer::BACKGROUND);
	background.push_back(index);
	collider.add_component(glm::vec2(drawable_size.x / 2.f, drawable_size.y - 20), glm::vec2(drawable_size.x, 40));

	index = tile_drawer.add_component(TileDrawer::Square{
		glm::vec2(drawable_size.x / 2, drawable_size.y - 160),
		glm::vec2(400, 40),
		glm::vec2(),
		glm::vec2()
	}, TileDrawer::MAP);
	tile_drawer.update_vertices(TileDrawer::MAP);
	map_components.push_back(index);
	collider.add_component(glm::vec2(drawable_size.x / 2, drawable_size.y - 160), glm::vec2(400, 40));

	index = tile_drawer.add_component(TileDrawer::Square{
		glm::vec2(drawable_size.x / 2 + 500, drawable_size.y - 250),
		glm::vec2(400, 40),
		glm::vec2(),
		glm::vec2()
	}, TileDrawer::MAP);
	tile_drawer.update_vertices(TileDrawer::MAP);
	map_components.push_back(index);
	collider.add_component(glm::vec2(drawable_size.x / 2 + 500, drawable_size.y - 250), glm::vec2(400, 40));

	index = tile_drawer.add_component(TileDrawer::Square{
		glm::vec2(drawable_size.x / 2 - 500, drawable_size.y - 250),
		glm::vec2(400, 40),
		glm::vec2(),
		glm::vec2()
	}, TileDrawer::MAP);
	tile_drawer.update_vertices(TileDrawer::MAP);
	map_components.push_back(index);
	collider.add_component(glm::vec2(drawable_size.x / 2 - 500, drawable_size.y - 250), glm::vec2(400, 40));

	index = tile_drawer.add_component(TileDrawer::Square{
		glm::vec2(drawable_size.x / 2 + 60, drawable_size.y - 480),
		glm::vec2(40, 400),
		glm::vec2(),
		glm::vec2()
	}, TileDrawer::MAP);
	tile_drawer.update_vertices(TileDrawer::MAP);
	map_components.push_back(index);
	collider.add_component(glm::vec2(drawable_size.x / 2 + 60, drawable_size.y - 480), glm::vec2(40, 400));

	index = tile_drawer.add_component(TileDrawer::Square{
		glm::vec2(drawable_size.x / 2 - 60, drawable_size.y - 400),
		glm::vec2(40, 400),
		glm::vec2(),
		glm::vec2()
	}, TileDrawer::MAP);
	tile_drawer.update_vertices(TileDrawer::MAP);
	map_components.push_back(index);
	collider.add_component(glm::vec2(drawable_size.x / 2 - 60, drawable_size.y - 400), glm::vec2(40, 400));

	index = tile_drawer.add_component(TileDrawer::Square{
		glm::vec2(drawable_size.x / 2 + 50, drawable_size.y - 60),
		glm::vec2(40, 80),
		glm::vec2(),
		glm::vec2()
	}, TileDrawer::CHARACTER);
	tile_drawer.update_vertices(TileDrawer::CHARACTER);
	self_index = index;

	player.position = tile_drawer.components[TileDrawer::CHARACTER][self_index].position;
	player.velocity = glm::vec2(0.f);
}

PlayMode::~PlayMode() {
}

bool PlayMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) {

	if (evt.type == SDL_KEYDOWN) {
		if (evt.key.repeat) {
			//ignore repeats
		} else if (evt.key.keysym.sym == SDLK_a) {
			left.downs += 1;
			left.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_d) {
			right.downs += 1;
			right.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_w) {
			up.downs += 1;
			up.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_s) {
			down.downs += 1;
			down.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_SPACE) {
			jump.downs += 1;
			jump.pressed = true;
		}
	} else if (evt.type == SDL_KEYUP) {
		if (evt.key.keysym.sym == SDLK_a) {
			left.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_d) {
			right.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_w) {
			up.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_s) {
			down.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_SPACE) {
			jump.pressed = false;
		}
	}

	return false;
}

void PlayMode::update(float elapsed) {
	bool collided;
	{
		// collision detection
		auto overlap = collider.solve_collision(player.position, glm::vec2(40.f, 80.f));
		collided = overlap.first;
	}

	glm::vec2 move = glm::vec2(0.f);
	if (left.pressed && !right.pressed) move.x = -horizontal_speed;
	if (!left.pressed && right.pressed) move.x = horizontal_speed;
	if (jump.pressed && collided) player.velocity.y = -jump_velocity;

	// update gravity
	player.position += move * elapsed;
	player.position += player.velocity * elapsed;
	player.velocity.y += gravity_acc * elapsed;

	{
		// collision resolution
		auto overlap = collider.solve_collision(player.position, glm::vec2(40.f, 80.f));

		if (overlap.first) {
			auto& resolve_vec = overlap.second;
			// has collision
			if (std::abs(resolve_vec.x) <= std::abs(resolve_vec.y)) {
				// use x dir
				player.velocity = glm::vec2(0.f); // eliminate velocity to stop on wall
				player.position.x += resolve_vec.x;
			} else {
				player.position.y += resolve_vec.y;
				if (resolve_vec.y >= 0) {
					// touched ceil, cut off excessive speed
					player.velocity.y = std::max(0.0001f, player.velocity.y);
				} else {
					// touched floor
					player.velocity.y = std::min(0.f, player.velocity.y);
				}
			}
		}
	}

	tile_drawer.components[TileDrawer::CHARACTER][self_index].position = player.position;

	//queue data for sending to server:
	//TODO: send something that makes sense for your game
	if (left.downs || right.downs || down.downs || up.downs) {
		//send a five-byte message of type 'b':
		client.connections.back().send('b');
		client.connections.back().send(left.downs);
		client.connections.back().send(right.downs);
		client.connections.back().send(down.downs);
		client.connections.back().send(up.downs);
	}

	//reset button press counters:
	left.downs = 0;
	right.downs = 0;
	up.downs = 0;
	down.downs = 0;

	//send/receive data:
	client.poll([this](Connection *c, Connection::Event event){
		if (event == Connection::OnOpen) {
			std::cout << "[" << c->socket << "] opened" << std::endl;
		} else if (event == Connection::OnClose) {
			std::cout << "[" << c->socket << "] closed (!)" << std::endl;
			throw std::runtime_error("Lost connection to server!");
		} else { assert(event == Connection::OnRecv);
			// std::cout << "[" << c->socket << "] recv'd data. Current buffer:\n" << hex_dump(c->recv_buffer); std::cout.flush();
			//expecting message(s) like 'm' + 3-byte length + length bytes of text:
			while (c->recv_buffer.size() >= 4) {
				// std::cout << "[" << c->socket << "] recv'd data. Current buffer:\n" << hex_dump(c->recv_buffer); std::cout.flush();
				char type = c->recv_buffer[0];
				if (type != 'm') {
					throw std::runtime_error("Server sent unknown message type '" + std::to_string(type) + "'");
				}
				uint32_t size = (
					(uint32_t(c->recv_buffer[1]) << 16) | (uint32_t(c->recv_buffer[2]) << 8) | (uint32_t(c->recv_buffer[3]))
				);
				if (c->recv_buffer.size() < 4 + size) break; //if whole message isn't here, can't process
				//whole message *is* here, so set current server message:
				server_message = std::string(c->recv_buffer.begin() + 4, c->recv_buffer.begin() + 4 + size);

				//and consume this part of the buffer:
				c->recv_buffer.erase(c->recv_buffer.begin(), c->recv_buffer.begin() + 4 + size);
			}
		}
	}, 0.0);
	
	// update vertice
	tile_drawer.update_vertices(TileDrawer::CHARACTER);
}

void PlayMode::draw(glm::uvec2 const &_drawable_size) {
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	drawable_size = {static_cast<float>(_drawable_size.x), static_cast<float>(_drawable_size.y)};
	tile_drawer.update_drawable_size(_drawable_size);
	tile_drawer.draw();

	{ //use DrawLines to overlay some text:
		glDisable(GL_DEPTH_TEST);
		float aspect = float(drawable_size.x) / float(drawable_size.y);
		DrawLines lines(glm::mat4(
			1.0f / aspect, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		));

		auto draw_text = [&](glm::vec2 const &at, std::string const &text, float H) {
			lines.draw_text(text,
				glm::vec3(at.x, at.y, 0.0),
				glm::vec3(H, 0.0f, 0.0f), glm::vec3(0.0f, H, 0.0f),
				glm::u8vec4(0x00, 0x00, 0x00, 0x00));
			float ofs = 2.0f / drawable_size.y;
			lines.draw_text(text,
				glm::vec3(at.x + ofs, at.y + ofs, 0.0),
				glm::vec3(H, 0.0f, 0.0f), glm::vec3(0.0f, H, 0.0f),
				glm::u8vec4(0xff, 0xff, 0xff, 0x00));
		};

		// draw_text(glm::vec2(-aspect + 0.1f, 0.0f), server_message, 0.09f);

		draw_text(glm::vec2(-aspect + 0.1f,-0.9f), "(press AD to move, press Space to jump)", 0.09f);
	}
	GL_ERRORS();
}
