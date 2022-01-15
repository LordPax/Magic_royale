#include "renderer/print.hpp"
#include "renderer/inputs.hpp"

std::string input(const std::string& prompt, bool same_line)
{
	std::cout << prompt << " " << End();
	std::string res;

	if (!same_line)
		std::cout << End(2);

	std::cout << cyan;
	std::getline(std::cin, res);
	return res;
}

void print_choices(const std::vector<std::string>& choices, const std::vector<std::string_view>& choice_colors, int choice_id)
{
	for (int i = 0; i < choices.size(); i++)
	{
		if (i == choice_id)
			std::cout << reverse << choice_colors[i] << " " << choices[i] << " " << reset << "  " << End();
		else
			std::cout << choice_colors[i] << " " << choices[i] << "   " << End();
	}
}

int choice(const std::vector<std::string>& choices, int initial_choice)
{
	return choice(choices, std::vector<std::string_view>(choices.size(), white), initial_choice);
}

int choice(const std::vector<std::string>& choices, const std::vector<std::string_view>& choice_colors, int initial_choice)
{
	Key key;
	int actual_choice = initial_choice;
	std::cout << End(1);
	print_choices(choices, choice_colors, actual_choice);

	while (true)
	{
		key = wait_for_key();

		if (key == Key::Left && actual_choice > 0)
		{
			actual_choice--;
			std::cout << '\r' << End();
			print_choices(choices, choice_colors, actual_choice);
		}

		if (key == Key::Right && actual_choice < choices.size() - 1)
		{
			actual_choice++;
			std::cout << '\r' << End();
			print_choices(choices, choice_colors, actual_choice);
		}

		if (key == Key::Enter)
			break;
	}

	std::cout << End(2);
	return actual_choice;
}

End::End(int nb_new_lines): nb_new_lines(nb_new_lines) {}

std::ostream& operator<<(std::ostream& os, const End& end)
{
	os << reset << std::flush;

	for (int i = 0; i < end.nb_new_lines; i++)
		os << std::endl;

	return os;
}

std::string_view get_color(Card::Color color)
{
	switch (color)
	{
	case Card::Color::Red:
		return red;
	case Card::Color::Green:
		return green;
	case Card::Color::Blue:
		return blue;
	case Card::Color::White:
		return white;
	case Card::Color::Black:
		return gray;
	case Card::Color::Colorless:
		return colorless;
	default:
		return "";
	}
}

void confirm()
{
	std::cout << std::endl;
	std::cin.get();
}

void clear_console()
{
	#if defined _WIN32
		system("cls");
	#elif defined (__LINUX__) || defined(__gnu_linux__) || defined(__linux__)
		system("clear");
	#elif defined (__APPLE__)
		system("clear");
	#endif
}