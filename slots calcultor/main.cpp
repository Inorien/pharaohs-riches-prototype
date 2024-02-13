#include <chrono>
#include <iostream>
#include <array>
#include <vector>

// Slots prototype:
//   Get some number of bonus steps to get some number of free spins
//	 Free spins bonus mode has different scoring and is the only way to win a rolling jackpot
//   Extend free spins?
// Tests:
//   TODO: Sort out the payout calculation so it can be montecarlo'd properly


class Slots {
public:

	unsigned spin() {

		char w1;
		char w2;
		char w3;

		std::srand(std::chrono::high_resolution_clock::now().time_since_epoch().count());
		auto rand = [this]() {
			const auto wheelSize{ bonus_mode ? bonus_wheel.size() : standard_wheel.size() };
			return std::rand() % wheelSize;
		};

		if (bonus_mode) {
			w1 = bonus_wheel[rand()];
			w2 = bonus_wheel[rand()];
			w3 = bonus_wheel[rand()];
		} else {
			w1 = standard_wheel[rand()];
			w2 = standard_wheel[rand()];
			w3 = standard_wheel[rand()];
		}

		print_state({w1, w2, w3}); //debug

		auto winnings{ check_win(w1, w2, w3) };

		if (bonus_steps == 10) {
			//bonus mode, give some free spins
			bonus_mode = true;
			bonus_steps = 0;
			remaining_bonus = free_spins; //debug

			for (int i = 0; i < free_spins; i++) {
				remaining_bonus--;
				winnings += spin();
			}

			bonus_mode = false;
		}

		return winnings;
	}

	unsigned check_win(char w1, char w2, char w3) {
		//todo: bonus_mode-specific logic
		// all wheels same value
		if ((w1 == w2) && (w2 == w3)) {
			//std::cout << "Win!";
			return 100;
		}
		if (w1 == 'X' || w2 == 'X' || w3 == 'X') {
			bonus_steps++;
		}
		return 0.0;
	}

	void print_state(std::array<char, 3> wheels) const {
		std::cout << wheels[0] << ' ' << wheels[1] << ' ' << wheels[2] << ' ';
		if (bonus_mode) {
			std::cout << "BONUS MODE! " << remaining_bonus << " free spins remain. ";
		} else {
			std::cout << "Bonus step: " << bonus_steps << ' ';
		}
		std::cout << std::endl;
	}

private:
	// X is bonus step symbol
	std::vector<char> standard_wheel{ 'A', 'B', 'C', 'D', 'E', 'X' };
	// Z is rolling jackpot symbol
	std::vector<char> bonus_wheel{ 'A', 'B', 'C', 'D', 'Z' };
	unsigned bonus_steps{ 0 };
	bool bonus_mode{false};
	unsigned char free_spins{ 10 };
	unsigned  remaining_bonus{ 0 }; //debug
};


int main() {

	size_t tests{ 10000 };
	std::cout << "Press Enter to spin, c to exit. Max " << tests << " spins\n";

	Slots slots;

	double totalWin{ 0.0 };
	int i = 0;
	while (int ch = getchar()) {
		if (ch == 'c' || i > tests) break;

		const auto result{ slots.spin() };
		totalWin += result;
		i++;
	}

	std::cout << "Total win: " << totalWin << " (played " << i << " rounds)" << std::endl;
	std::cout << "Average payout: " << totalWin * 100 / static_cast<double>(i) << '%' << std::endl;

	return 0;
}