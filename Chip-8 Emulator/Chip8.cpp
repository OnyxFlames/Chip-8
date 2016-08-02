#include "Chip8.hpp"

#include <bitset>
#include <iostream>
#include <iomanip>

Chip8::Chip8()
{
	window.create(sf::VideoMode(64 * pixel_size, 32 * pixel_size), "Chip-8 - NO ROM");
	init_all(monitor, pixel_size, sf::Color(127, 127, 127));
}

void Chip8::run()
{
		while (window.isOpen() && !ROM.eof())
		{
			read();
			input();
			update();
			draw();
		}
}
bool Chip8::load_rom(const std::string ROM_location)
{
	ROM.open(ROM_location);
	if (!ROM.is_open())
	{
		window.setTitle("Chip-8 - Error loading ROM");
	}
	else
	{
		window.setTitle("Chip-8 - " + ROM_location);
	}
	return ROM.is_open();
}
void Chip8::input()
{
	while (window.pollEvent(event))
	{
		// TODO: Make closing the emulator cleaner once more progress has been made.
		if (event.type == sf::Event::Closed || sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
			window.close();
		/*
			TODO: Put code that loads keyboard input into the registers
		*/
	}
}
void Chip8::draw()
{
	window.clear();
	for (auto _ : monitor)
		for (auto __ : _)
			window.draw(__);
	window.display();
}
void Chip8::update()
{
	// TODO: include check for sound and delay clocks here
}

void Chip8::read()
{
	byte instruct_buff[2];
	
	instruct_buff[0] = ROM.get(); instruct_buff[1] = ROM.get();

	std::cout << "Buffer: " << std::hex  << std::setfill('0') << std::setw(2) << (int)instruct_buff[0] << std::setfill('0') << std::setw(2) << (int)instruct_buff[1] << "\n";
	
	if (left_nibble(instruct_buff[0]) == 0)
	{
		if (instruct_buff[1] == 0xE0)	// opcode is 0x00E0
		{
			op_clear_screen();
		}
		else if (instruct_buff[1] == 0xEE)	// return from subroutine
		{
			if (stack.empty())
			{
				std::cout << "ATTEMPTED TO RETURN FROM UNKNOWN SUBROUTINE. Please fix.\n";
				std::exit(-1);
			}
			ROM.seekg(stack.top());
			stack.pop();
			// TODO: Implement subroutine calls 
		}
	}
	else if (left_nibble(instruct_buff[0] == 1))	// jump instruction
	{
		// TODO: Test jump opcode
		std::cout << "JUMP CALLED\n";
		short jmp = 0x0000;
		//std::cout << "Debug sub: " << std::bitset<16>(jmp) << " " << std::bitset<16>(jmp).to_ulong() << "\n";
		jmp = (right_nibble(instruct_buff[0]) << 4);
		//std::cout << "Debug sub: " << std::bitset<16>(jmp) << " " << std::bitset<16>(jmp).to_ulong() << "\n";
		jmp <<= 12;
		//std::cout << "Debug sub: " << std::bitset<16>(jmp) << " " << std::bitset<16>(jmp).to_ulong() << "\n";
		jmp += instruct_buff[1];
		ROM.seekg(jmp);
	}
	else if (left_nibble(instruct_buff[0]) == 2)	// call subroutine
	{
		stack.push((short)ROM.tellg());
		short jmp = 0x0000;
		std::cout << "Debug sub: " << std::bitset<16>(jmp) << " " << std::bitset<16>(jmp).to_ulong() << "\n";
		jmp = (right_nibble(instruct_buff[0]) << 4);
		std::cout << "Debug sub: " << std::bitset<16>(jmp) << " " << std::bitset<16>(jmp).to_ulong() << "\n";
		jmp <<= 12;
		std::cout << "Debug sub: " << std::bitset<16>(jmp) << " " << std::bitset<16>(jmp).to_ulong() << "\n";
		jmp += instruct_buff[1];
		ROM.seekg(jmp);
	}
	else if (left_nibble(instruct_buff[0]) == 3)	// if VX == NN skip the next two
	{
		if (registers[right_nibble(instruct_buff[0])] == instruct_buff[1])
		{
			ROM.get(); ROM.get();
		}
	}
	else if (left_nibble(instruct_buff[0]) == 4)	// if VX != NN skip the next two
	{
		if (registers[right_nibble(instruct_buff[0])] != instruct_buff[1])
		{
			ROM.get(); ROM.get();
		}
	}
	else if (left_nibble(instruct_buff[0]) == 5)	// if VX == VY skip the next two
	{
		if (registers[right_nibble(instruct_buff[0])] == registers[left_nibble(instruct_buff[1])])
		{
			ROM.get(); ROM.get();
		}
	}
	else if (left_nibble(instruct_buff[0]) == 6)	// sets VX to NN
	{
		registers[right_nibble(instruct_buff[0])] = instruct_buff[1];
	}
	else if (left_nibble(instruct_buff[0]) == 7)	// adds NN to VX
	{
		registers[right_nibble(instruct_buff[0])] += instruct_buff[1];
	}
	else if (left_nibble(instruct_buff[0]) == 8)
	{
		if (right_nibble(instruct_buff[1]) == 0)	// set VX to the value of VY
		{
			registers[right_nibble(instruct_buff[0])] = registers[left_nibble(instruct_buff[1])];
		}
		else if (right_nibble(instruct_buff[1]) == 1)
		{
			registers[right_nibble(instruct_buff[0])] |= registers[left_nibble(instruct_buff[1])];
		}
		else if (right_nibble(instruct_buff[1]) == 2)
		{
			registers[right_nibble(instruct_buff[0])] &= registers[left_nibble(instruct_buff[1])];
		}
		else if (right_nibble(instruct_buff[1]) == 3)
		{
			registers[right_nibble(instruct_buff[0])] ^= registers[left_nibble(instruct_buff[1])];
		}
		else if (right_nibble(instruct_buff[1]) == 4)
		{
			// TODO: add with carry opcode
		}
		else if (right_nibble(instruct_buff[1]) == 5)
		{
			// TODO: subtract with borrow opcode
		}
		else if (right_nibble(instruct_buff[1]) == 6)
		{
			// TODO: shift VX right by one -- VF equals last bit of VX before shift
		}
		else if (right_nibble(instruct_buff[1]) == 7)
		{
			// TODO: set VX to VY - VX -- VF is set to 0 when there is a borrow 1 when there isn't
		}
		else if (right_nibble(instruct_buff[1]) == 14)
		{
			// TODO: shifts VX left by one -- VF is set to the value of the first bit of VX before shift
		}
	}
	else if (left_nibble(instruct_buff[0]) == 9)
	{
		if (right_nibble(instruct_buff[0]) != left_nibble(instruct_buff[1]))
		{
			ROM.get(); ROM.get();
		}
	}

}

void Chip8::op_clear_screen()
{
	color_pixels(monitor, sf::Color::Black);
}
Chip8::~Chip8()
{
}
