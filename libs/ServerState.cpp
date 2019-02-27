#include "ServerState.hpp"
#include "ServerRoom.hpp"

namespace cp {
	ServerState::ServerState(GameDataRef _data, const std::vector<Client*> &clients)
		: game_data(_data), simulator(_data), fout("ServerState.log"), clients(clients) {
		fout<<"Executing ServerState"<<std::endl;
		// for(auto &client:this->clients) {
		// 	std::cout<<client->get_identity()<<std::endl;
		// }
		fout<<"Returning from ServerState"<<std::endl;
	}

	ServerState::~ServerState() {
		fout<<"Closed"<<std::endl;
		fout.close();
	}

	void ServerState::collect_network_inputs() {
		for(auto &client:clients) {
			inputs.push_back(client->get_inputs());
		}
	}

	void ServerState::use_collected_inputs() {
		for(auto &input:inputs) {
			game_data->input.register_input(input);
		}
	}

	void ServerState::handle_input(float delta) {
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::L)) {
			fout<<"Log register in handle_input"<<std::endl;
		}
		collect_network_inputs();
		use_collected_inputs();
		simulator.handle_input(delta);
	}

	void ServerState::init() {
		fout<<"Executing init"<<std::endl;
		fout<<"Connecting all the clients"<<std::endl;
		simulator.init();
		for(auto &client:clients)
			simulator.add_external_player(client->get_identity());
		simulator.update_main_player(12312);
		fout<<"Returning from init"<<std::endl;
	}

	void ServerState::draw(float delta) {

		simulator.draw(delta);
	}

	void ServerState::update(float delta) {
		simulator.update(delta);
		generate_outputs();
		use_generated_outputs();
	}

	void ServerState::generate_outputs() {
		temp_snap = simulator.get_current_snap(SnapFlag::NETWORK_SNAP);
	}

	void ServerState::use_generated_outputs() {
		for(auto &client:clients) {
			client->send_snap(temp_snap);
		}
	}
}