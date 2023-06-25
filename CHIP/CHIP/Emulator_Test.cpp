#include <SDL.h>
#include "CHIP/Chip.h"
#include "Include/clove-unit.h"
chipotto::Emulator* emulator = nullptr;

CLOVE_SUITE_SETUP_ONCE() { //Only at Start

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
	{
		SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
	}

}

CLOVE_SUITE_SETUP() { //Before each test

	emulator = new chipotto::Emulator();

}

CLOVE_SUITE_TEARDOWN_ONCE() {

	SDL_Quit();
}

CLOVE_TEST(Opcode0_Clear_Test) {

	uint8_t* pixel = nullptr;
	int pitch;
	emulator->OpcodeD(0xD005);

	CLOVE_INT_EQ((int)chipotto::OpcodeStatus::IncrementPC, (int)emulator->Opcode0(0x00E0));

	SDL_LockTexture(emulator->GetTexture(), nullptr, reinterpret_cast<void**>(&pixel), &pitch);

	uint8_t* resultPixels = static_cast<uint8_t*>(calloc(pitch * emulator->GetHeightTexture(), sizeof(uint8_t)));
	CLOVE_NOT_NULL(resultPixels);

	CLOVE_INT_EQ(0, memcmp(resultPixels, pixel, emulator->GetHeightTexture() * pitch * sizeof(uint8_t)));

	free(resultPixels);
}

CLOVE_TEST(Opcode0_Overflow_Test) {

	CLOVE_INT_EQ((int)chipotto::OpcodeStatus::StackOverflow, (int)emulator->Opcode0(0x00EE));
}

CLOVE_TEST(Opcode0_Ret_Test) {

	emulator->Opcode2(0x2380);
	CLOVE_INT_EQ((int)chipotto::OpcodeStatus::IncrementPC, (int)emulator->Opcode0(0x00EE));
	CLOVE_INT_EQ(emulator->GetPC(), 0x200);
}

CLOVE_TEST(Opcode1_Test) {

	CLOVE_INT_EQ((int)chipotto::OpcodeStatus::IncrementPC, (int)emulator->Opcode1(0x1666));
	CLOVE_INT_EQ(emulator->GetPC(), 0x666 - 2);
}

CLOVE_TEST(Opcode2_Test) {

	for (size_t i = 0; i < 16; i++)
	{
		emulator->Opcode2(0x2066);
	}

	CLOVE_INT_EQ((int)chipotto::OpcodeStatus::StackOverflow, (int)emulator->Opcode2(0x2066));
	CLOVE_INT_EQ(emulator->GetStack(), emulator->GetPC());
	CLOVE_INT_EQ(emulator->GetPC(), 0x66);
}

CLOVE_TEST(Opcode4_Test) {

	emulator->Opcode6(0x6099);
	CLOVE_INT_EQ((int)chipotto::OpcodeStatus::IncrementPC, (int)emulator->Opcode4(0x4066));
	CLOVE_INT_EQ(emulator->GetPC(), 0x200 + 2);
}

CLOVE_TEST(Opcode6_Test) {

	CLOVE_INT_EQ((int)chipotto::OpcodeStatus::IncrementPC, (int)emulator->Opcode6(0x6066));
	CLOVE_INT_EQ(emulator->GetIndexRegister(0x0), 0x66);
}
CLOVE_TEST(Opcode7_Test) {

	CLOVE_INT_EQ((int)chipotto::OpcodeStatus::IncrementPC, (int)emulator->Opcode7(0x7066));
	CLOVE_INT_EQ(emulator->GetIndexRegister(0x0), 0x66);
}

CLOVE_TEST(Opcode8_LD_Test) {

	CLOVE_INT_EQ((int)chipotto::OpcodeStatus::IncrementPC, (int)emulator->Opcode8(0x8010));
	CLOVE_INT_EQ(emulator->GetIndexRegister(0x0), emulator->GetIndexRegister(0x1));
}

CLOVE_TEST(Opcode8_AND_Test) {

	CLOVE_INT_EQ((int)chipotto::OpcodeStatus::IncrementPC, (int)emulator->Opcode8(0x8012));
	CLOVE_INT_EQ(emulator->GetIndexRegister(0x0), 0);
}

CLOVE_TEST(Opcode8_XOR_Test) {

	CLOVE_INT_EQ((int)chipotto::OpcodeStatus::IncrementPC, (int)emulator->Opcode8(0x8013));
	CLOVE_INT_EQ(emulator->GetIndexRegister(0x0), 0);
}

CLOVE_TEST(Opcode8_ADD_Test) {

	CLOVE_INT_EQ((int)chipotto::OpcodeStatus::IncrementPC, (int)emulator->Opcode8(0x8014));
	CLOVE_INT_EQ(emulator->GetIndexRegister(0x0),0);
}

CLOVE_TEST(Opcode8_SHL_Test) {

	emulator->Opcode6(0x6066);
	CLOVE_INT_EQ((int)chipotto::OpcodeStatus::IncrementPC, (int)emulator->Opcode8(0x801E));
	CLOVE_INT_EQ(emulator->GetIndexRegister(0xF),0);
	CLOVE_INT_EQ(emulator->GetIndexRegister(0x0), 204);

}

CLOVE_TEST(OpcodeA_Test) {

	CLOVE_INT_EQ((int)chipotto::OpcodeStatus::IncrementPC, (int)emulator->OpcodeA(0xA321));
	CLOVE_INT_EQ(emulator->GetI(), 0x321);
}

CLOVE_TEST(OpcodeC_Test) {

	std::srand(0);
	CLOVE_INT_EQ((int)chipotto::OpcodeStatus::IncrementPC, (int)emulator->OpcodeC(0xC0FF));
	CLOVE_INT_EQ(emulator->GetIndexRegister(0x0),38);
}

CLOVE_TEST(OpcodeF_55_Test) {

	CLOVE_INT_EQ((int)chipotto::OpcodeStatus::IncrementPC, (int)emulator->OpcodeF(0xF255));
	
	CLOVE_INT_EQ(emulator->GetIndexMemoryMap(0), 0);
	CLOVE_INT_EQ(emulator->GetIndexMemoryMap(1), 0);
	CLOVE_INT_EQ(emulator->GetIndexMemoryMap(2), 0);
}

CLOVE_TEST(OpcodeF_65_Test) {

	CLOVE_INT_EQ((int)chipotto::OpcodeStatus::IncrementPC, (int)emulator->OpcodeF(0xF465));

	CLOVE_INT_EQ(emulator->GetIndexRegister(0), 0xF0);
	CLOVE_INT_EQ(emulator->GetIndexRegister(1), 0x90);
	CLOVE_INT_EQ(emulator->GetIndexRegister(2), 0x90);
	CLOVE_INT_EQ(emulator->GetIndexRegister(3), 0x90);
	CLOVE_INT_EQ(emulator->GetIndexRegister(4), 0xF0);
}

CLOVE_TEST(OpcodeF_33_Test) {

	emulator->Opcode6(0x6090);
	CLOVE_INT_EQ((int)chipotto::OpcodeStatus::IncrementPC, (int)emulator->OpcodeF(0xF033));

	CLOVE_INT_EQ(emulator->GetIndexMemoryMap(0x0), 1);
	CLOVE_INT_EQ(emulator->GetIndexMemoryMap(1), 134);
	CLOVE_INT_EQ(emulator->GetIndexMemoryMap(2), 4); 
}

CLOVE_TEST(OpcodeF_29_Test) {

	emulator->Opcode6(0x6090);
	CLOVE_INT_EQ((int)chipotto::OpcodeStatus::IncrementPC, (int)emulator->OpcodeF(0xF029));

	CLOVE_INT_EQ(emulator->GetI(), 5*0x90);
}

CLOVE_TEST(OpcodeF_0A_Test) {

	CLOVE_INT_EQ((int)chipotto::OpcodeStatus::WaitForKeyboard, (int)emulator->OpcodeF(0xF00A));

	CLOVE_INT_EQ(emulator->GetKeyboardRegIndex(), 0);
}

CLOVE_TEST(OpcodeF_1E_Test) {

	emulator->Opcode6(0x6033);
	CLOVE_INT_EQ((int)chipotto::OpcodeStatus::IncrementPC, (int)emulator->OpcodeF(0xF01E));

	CLOVE_INT_EQ(emulator->GetI(), 0x33);
}

CLOVE_TEST(OpcodeF_18_Test) {

	CLOVE_INT_EQ((int)chipotto::OpcodeStatus::IncrementPC, (int)emulator->OpcodeF(0xF018));
}

CLOVE_TEST(OpcodeF_15_Test) {

	emulator->Opcode6(0x6033);

	CLOVE_INT_EQ((int)chipotto::OpcodeStatus::IncrementPC, (int)emulator->OpcodeF(0xF015));

	CLOVE_INT_EQ(emulator->GetDelayTimer(), 0x33);
	CLOVE_INT_EQ(emulator->GetDeltaTimerTicks(), 17 + SDL_GetTicks64());
}

CLOVE_TEST(OpcodeF_07_Test) {

	//emulator->Opcode6(0x6033);

	CLOVE_INT_EQ((int)chipotto::OpcodeStatus::IncrementPC, (int)emulator->OpcodeF(0xF007));

	CLOVE_INT_EQ(emulator->GetIndexRegister(0x0), emulator->GetDelayTimer());
}

CLOVE_SUITE_TEARDOWN() { //End of each test

	delete emulator;
}