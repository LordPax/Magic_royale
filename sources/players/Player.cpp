#include "players/Player.hpp"
#include "Game.hpp"

Player::Player(): m_name(""), m_health(20), m_alive(true) {}

std::string Player::get_name() const
{
	return m_name;
}

void Player::set_name(const std::string& name)
{
	m_name = name;
}

int Player::get_health() const
{
	return m_health;
}

void Player::set_health(int health)
{
	m_health = health;
}

void Player::reduce_health(int amount)
{
	m_health -= amount;

	if (m_health <= 0)
		m_alive = false;
}

Player::Resources Player::get_resources()
{
	Resources res =
	{
		{ Card::Color::Colorless, 0 },
		{ Card::Color::White, 0 },
		{ Card::Color::Blue, 0 },
		{ Card::Color::Black, 0 },
		{ Card::Color::Red, 0 },
		{ Card::Color::Green, 0 }
	};

	for (auto& land : m_lands)
	{
		if (!land.is_engaged())
		{
			res[Card::Color::Colorless]++;
			res[land.get_color()]++;
		}
	}

	return res;
}

PtrList<Card> Player::get_deck()
{
	return m_deck;
}

void Player::create_deck(const PtrList<Card>& deck)
{
	m_deck = deck;

	size_t index_rand;
	srand ((unsigned int)time(NULL));

	// Mélange les cartes
	for (size_t i = m_deck.size() - 1; i > 0; i--)
	{
		index_rand = rand() % i;
		m_deck.swap(i, index_rand);
	}

	m_library = m_deck;
	for(int i = 0; i < 7; i++)
		draw_card();
}

// séparer en play_creature / play_land ?
void Player::play_card(const Card& card)
{
	if (card.get_type() == Card::Type::Creature)
	{
		m_creatures.add(card);
		m_creatures.back().spawn();
	}

	else
	{
		m_lands.add(card);
	}
	m_hand.remove(card);
	// TODO : set_player()
}

bool Player::is_creature_playable(const Creature& creature)
{
	Card::Cost cost_map = creature.get_cost();
	Resources resources_map = get_resources();
	Resources to_engage =
	{
		{ Card::Color::Colorless, 0 },
		{ Card::Color::White, 0 },
		{ Card::Color::Blue, 0 },
		{ Card::Color::Black, 0 },
		{ Card::Color::Red, 0 },
		{ Card::Color::Green, 0 }
	};

	for (auto& [color, cost] : cost_map)
		if (!(color == Card::Color::Colorless) && cost > 0)
		{
			if (cost > resources_map[color])
				return false;

			else
			{
				resources_map[color] -= cost;
				resources_map[Card::Color::Colorless] -= cost;
				to_engage[color] += cost;
			}
		}

	if (cost_map[Card::Color::Colorless] > resources_map[Card::Color::Colorless])
		return false;

	else
	{
		resources_map[Card::Color::Colorless] -= cost_map[Card::Color::Colorless];
		to_engage[Card::Color::Colorless] += cost_map[Card::Color::Colorless];
	}

    // TODO : mettre tout ça dans une autre fonction ?

	size_t number;

	while (to_engage[Card::Color::Colorless] > 0)
	{
		// TODO : afficher uniquement si le nb de terrains est > 0 et avec la bonne orthographe si = 1
		std::cout << "Choisissez un terrain a engager." << std::endl;
		std::cout << "Vous devez encore engager " << to_engage[Card::Color::White] << " terrains blancs, ";
		std::cout << to_engage[Card::Color::Blue] << " terrains bleus, ";
		std::cout << to_engage[Card::Color::Black] << " terrains noirs, ";
		std::cout << to_engage[Card::Color::Red] << " terrains rouges, ";
		std::cout << to_engage[Card::Color::Green] << " terrains verts et ";
		std::cout << to_engage[Card::Color::Colorless] << " autres terrains de n'importe quelle couleur." << std::endl << std::endl;

		for(int i = 0; i < m_lands.size(); i++)
		{
			std::cout << m_lands[i].get_name() << " " << i + 1 << std::endl;
		}

		// TODO : afficher les numeros correspondant aux terrains que l'on peut engager
		std::cin >> number;
		number--;
		if (number > m_lands.size())
			std::cout << "Vous avez entre un numero incorrect." << std::endl;

		else if (m_lands[number].is_engaged())
			std::cout << "Ce terrain est deja engage." << std::endl;

		else
		{
			m_lands[number].engage();
			if(to_engage[m_lands[number].get_color()] > 0)
				to_engage[m_lands[number].get_color()] -= 1;
			else if(to_engage[Card::Color::Colorless] > 0)
				to_engage[Card::Color::Colorless] -= 1;
			else
				std::cout << "Vous n'avez pas besoin d'engager ce terrain." << std::endl;
		}
	}

	return true;
}

void Player::begin_turn()
{
	draw_card();
	disengage_cards();
	main_phase();
}

void Player::draw_card()
{
	m_hand.add(m_library.back());
	m_library.remove(m_library.back());
	// TODO : si plus de cartes à piocher, le joueur perd la partie
	// TODO : si déjà 7 cartes en main, défausser la carte (-> cimetière)
}

void Player::disengage_cards()
{
	for (auto& creature : m_creatures)
		if (creature.is_engaged())
			creature.disengage();

	for (auto& land : m_lands)
		if (land.is_engaged())
			land.disengage();
}

void Player::main_phase()
{
	bool stop = false;
	size_t number;

	while (!stop && m_hand.size() > 0)
	{
		std::cout << std::endl << "Phase principale" << std::endl;
		std::cout << "Veuillez selectionner une carte que vous voulez jouer en entrant son numero." << std::endl;
		std::cout << "Si vous avez fini de placer des cartes, tapez 0." << std::endl << std::endl;

		for(int i = 0; i < m_hand.size(); i++)
		{
			std::cout << m_hand[i].get_name() << " " << i + 1 << std::endl;
		}

		std::cin >> number;

		if (number == 0)
			stop = true;
		else
		{
			number--;
			if (number > m_hand.size())
				std::cout << "Vous avez entre un numero invalide." << std::endl;

			else
			{
				if (m_hand[number].get_type() == Card::Type::Creature)
				{
					Creature& creature = (Creature&)m_hand[number];
					if (!is_creature_playable(creature))
						std::cout << "Vous n'avez pas suffisamment de terrains pour jouer cette carte." << std::endl;
					else
						play_card(creature);
				}

				else
					play_card(m_hand[number]);
			}
		}
	}

	combat_phase();
}

void Player::combat_phase()
{
	// TODO

	secondary_phase();
}

void Player::secondary_phase()
{
	// TODO

	end_turn();
}

void Player::end_turn()
{
	// TODO
}

void Player::play()
{
	// TODO
}

void Player::reduce_creatures_health(int amount)
{
	for (auto& creature : m_creatures)
		creature.reduce_toughness(amount);
}

Player& Player::get_opponent() const
{
	if (&Game::players[0] == this)
		return Game::players[1];

	return Game::players[0];
}

void Player::add_creature(const Creature& creature)
{
	m_creatures.add(creature);
}
