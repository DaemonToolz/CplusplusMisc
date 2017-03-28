// Misc.h

#pragma once
#include <iostream>
#include <math.h>
#include <bitset>
#include <vector>
using namespace std;
using namespace System;

typedef struct TwoBytesInt {
	unsigned  b : 2;
} Tbi;

namespace Misc {
	namespace Crypto {

		public class SDES {
		private:
			vector<vector<bitset<2> >* > sandbox_1, sandbox_2;


		public:
			bitset<10> master;

			void changeBitAt(int sb, int row, int col, TwoBytesInt tbi) {
				(sb == 1 ? sandbox_1 : sandbox_2).at(row)->at(col) = bitset<2>(tbi.b);
			}

			void updateSandbox(int sb, int row, int col, int value) {
				TwoBytesInt tbi;

				tbi.b = value;
				changeBitAt(sb, row, col, tbi);
			}

			void initializeSandboxes(bool random = true) {
				for (int i = 0; i < 4; ++i) {
					sandbox_1.push_back(new vector<bitset<2>>(4));
					sandbox_2.push_back(new vector<bitset<2>>(4));
				}

				int sbIndex = 1;
				/**/
				if (random)
					for (auto i = 0; i < 4; ++i, sbIndex++)
						for (auto j = 0; j < 4; ++j) {
							updateSandbox(sbIndex, j, i, (rand() % 3));
						}
				else {

					// Sandbox 0
					updateSandbox(sbIndex, 0, 0, 1);
					updateSandbox(sbIndex, 1, 0, 3);
					updateSandbox(sbIndex, 2, 0, 0);
					updateSandbox(sbIndex, 3, 0, 3);

					updateSandbox(sbIndex, 0, 1, 0);
					updateSandbox(sbIndex, 1, 1, 2);
					updateSandbox(sbIndex, 2, 1, 2);
					updateSandbox(sbIndex, 3, 1, 1);

					updateSandbox(sbIndex, 0, 2, 3);
					updateSandbox(sbIndex, 1, 2, 1);
					updateSandbox(sbIndex, 2, 2, 1);
					updateSandbox(sbIndex, 3, 2, 3);

					updateSandbox(sbIndex, 0, 3, 2);
					updateSandbox(sbIndex, 1, 3, 0);
					updateSandbox(sbIndex, 2, 3, 3);
					updateSandbox(sbIndex++, 3, 3, 2);

					// Sandbox 1
					updateSandbox(sbIndex, 0, 0, 0);
					updateSandbox(sbIndex, 1, 0, 2);
					updateSandbox(sbIndex, 2, 0, 3);
					updateSandbox(sbIndex, 3, 0, 2);

					updateSandbox(sbIndex, 0, 1, 1);
					updateSandbox(sbIndex, 1, 1, 0);
					updateSandbox(sbIndex, 2, 1, 0);
					updateSandbox(sbIndex, 3, 1, 1);

					updateSandbox(sbIndex, 0, 2, 2);
					updateSandbox(sbIndex, 1, 2, 1);
					updateSandbox(sbIndex, 2, 2, 1);
					updateSandbox(sbIndex, 3, 2, 0);

					updateSandbox(sbIndex, 0, 3, 3);
					updateSandbox(sbIndex, 1, 3, 3);
					updateSandbox(sbIndex, 2, 3, 0);
					updateSandbox(sbIndex++, 3, 3, 3);
				}
			}

			void initializeKey(bitset<10> input) {
				master = input;
			}
			// P10 (k1, k2, k3, k4, k5, k6, k7, k8, k9, k10 ) = (k3, k5, k2, k7, k4, k10, k1, k9, k8, k6)

#pragma region Permutations
			template<unsigned int T, unsigned int KS>
			bitset<T> permutationIntBased(bitset<KS> key, unsigned int bits) {
				bitset<T> internalkey;
				bitset<KS> transition = bitset(bits);
				for (int i = 0; i < T; i++) {
					internalkey.set(i, transition[i]);
				}
				return internalkey;
			}

			template<unsigned int T, unsigned int KS>
			bitset<T> permutationBitBased(bitset<KS> key, vector<bool> bitvector) {
				bitset<T> internalkey;
				for (int i = 0; i < T; i++) {
					internalkey.set(i, bitvector.at(i));
				}
				return internalkey;
			}

			bitset<10> p10(bitset<10> key) {
				bitset<10> internalkey = key;
				int temp = 0;

				internalkey.set(temp++, key[2]); // +2
				internalkey.set(temp++, key[4]); // +2
				internalkey.set(temp++, key[1]); // -1
				internalkey.set(temp++, key[6]); // +5
				internalkey.set(temp++, key[3]); // -3
				internalkey.set(temp++, key[9]); // +6
				internalkey.set(temp++, key[0]); // -9
				internalkey.set(temp++, key[8]); // +8
				internalkey.set(temp++, key[7]); // -1
				internalkey.set(temp++, key[5]); // -2

				return internalkey;
			}

			// P8(1 2 3 4 5 6 7 8 9 10) = (6 3 7 4 8 5 10 9 )
			bitset<8> p8(bitset<10> key) {
				bitset<8> internalkey;
				int temp = 0;


				internalkey.set(temp++, key[5]); // +5
				internalkey.set(temp++, key[2]); // -3
				internalkey.set(temp++, key[6]); // +4
				internalkey.set(temp++, key[3]); // -3
				internalkey.set(temp++, key[7]); // +4
				internalkey.set(temp++, key[4]); // -3
				internalkey.set(temp++, key[9]); // +6
				internalkey.set(temp++, key[8]); // -1



				return internalkey;
			}

			bitset<2> fromSandbox(int sb, bitset<2> x, bitset<2> y) {
				return (sb == 1 ? sandbox_1 : sandbox_2).at((int)(x.to_ulong()))->at((int)(y.to_ulong()));
			}
			//P4 (s00, s01, s10, s11) = (s01, s11, s10, s00)
			// XOR  1111 0001
			// S0[11][11] -> 10    S1[01][00] -> 10

			bitset<4> p4(bitset<8> key) {
				bitset<4> internalkey;
				bitset<2> temp;

				bitset<2> x, y;

				for (int part = 0; part < 2; part++) {
					if (part == 0) {
						x.set(0, key[0]); // 00  sb 1
						x.set(1, key[3]); // 01
						y.set(0, key[1]);
						y.set(1, key[2]);
					}
					else {
						x.set(0, key[4]);
						x.set(1, key[7]);
						y.set(0, key[5]);
						y.set(1, key[6]);
					}

					temp = fromSandbox(part, x, y); // 01
					for (int i = 0; i < 2; ++i)
						internalkey.set((part * 2) + i, temp[i]); // 00 01
				}

				return internalkey;
			}

#pragma endregion

#pragma region Functions


			bitset<4> f(bitset<4> fragment, bitset<8> sk) {
				return p4(E_P(fragment) ^ sk);
			}

			bitset<8> fk(bitset<8> bits, bitset<8> key) {
				bitset<4> left, right;
				for (int i = 0; i < 4; ++i) {
					left.set(i, bits[i]);
					right.set(i, bits[4 + i]);
				}

				left ^= f(right, key);
				bitset<8> finalb;
				for (int i = 0; i < 4; ++i) {
					finalb.set(i, left[i]);
					finalb.set(i + 4, right[i]);
				}

				return finalb;
			}

			bitset<8> sw(bitset<8>& input) {
				bitset<8> internalkey = input;
				bitset<4> save;
				for (int i = 0; i < 4; ++i) {
					save.set(i, internalkey[i]);
					internalkey.set(i, internalkey[i + 4]);
					internalkey.set(i + 4, save[i]);
				}

				return internalkey;
			}


#pragma endregion

#pragma region Initial Permutations
			//IP(k1, k2, k3, k4, k5, k6, k7, k8) = (k2, k6, k3, k1, k4, k8, k5, k7)

			bitset<8> ip(bitset<8> key) {
				bitset<8> internalkey;
				int temp = 0;
				internalkey.set(temp++, key[1]); // +1
				internalkey.set(temp++, key[5]); // +4
				internalkey.set(temp++, key[2]); // -3
				internalkey.set(temp++, key[0]); // -2
				internalkey.set(temp++, key[3]); // +3
				internalkey.set(temp++, key[7]); // +4
				internalkey.set(temp++, key[4]); // -3
				internalkey.set(temp++, key[6]); // +2


				return internalkey;
			}

			//IP−1(k1, k2, k3, k4, k5, k6, k7, k8) = (k4, k1, k3, k5, k7, k2, k8, k6)
			bitset<8> iip(bitset<8> key) {
				bitset<8> internalkey;
				int temp = 0;
				internalkey.set(temp++, key[3]); // +3
				internalkey.set(temp++, key[0]); // -3
				internalkey.set(temp++, key[2]); // +2
				internalkey.set(temp++, key[4]); // +2
				internalkey.set(temp++, key[6]); // +2
				internalkey.set(temp++, key[1]); // -5
				internalkey.set(temp++, key[7]); // +6
				internalkey.set(temp++, key[5]); // -2

				return internalkey;
			}

#pragma endregion


			bitset<10> leftShift(bitset<10> key, int shift = 1) {
				bitset<10> internalkey = key;

				auto tail = internalkey[0], head = internalkey[5];
				for (int i = 3; i > 0; --i) {
					internalkey.set(i, internalkey[i + shift]);
					internalkey.set(5 - (shift - 1 + i), internalkey[9 - (shift - 1 + i)]);
				}

				internalkey.set(4, tail);
				internalkey.set(9, head);

				return internalkey;
			}

			// E/P (n1, n2, n3, n4) = (n4, n1, n2, n3, n2, n3, n4, n1)
			bitset<8> E_P(bitset<4> key) {
				bitset<8> internalkey;
				auto temp = 0;

				internalkey.set(temp++, key[3]);
				internalkey.set(temp++, key[0]);
				internalkey.set(temp++, key[1]);
				internalkey.set(temp++, key[2]);

				internalkey.set(temp++, key[1]);
				internalkey.set(temp++, key[2]);
				internalkey.set(temp++, key[3]);
				internalkey.set(temp++, key[0]);

				return internalkey;
			}

			bitset<8> xor (bitset<8> skey_1, bitset<8> skey_2) {
				return skey_1 ^ skey_2;
			}


			vector<bitset<8>> generateKeys() {
				vector<bitset<8>> vectors;

				vectors.push_back(p8(leftShift(p10(master))));
				vectors.push_back(p8(leftShift(leftShift(leftShift(p10(master))))));

				return vectors;
			}

			char encrypt(unsigned char block) {
				vector<bitset<8>> keys = generateKeys();
				return static_cast<unsigned char>(iip(fk(sw(fk(ip(bitset<8>(block)), keys.at(0))), keys.at(1))).to_ulong());
			}


			char decrypt(unsigned char block) {
				vector<bitset<8>> keys = generateKeys();
				return static_cast<unsigned char>(iip(fk(sw(fk(ip(bitset<8>(block)), keys.at(1))), keys.at(0))).to_ulong());
			}
		};

	}

	namespace Math {
		public ref class XMath{
		public :
			int bitwise_mul_2(int value) {
				return value << 1;
			}

			int bitwise_div_2(int value) {
				return value >> 1;
			}

			int bitwise_add(int x, int y){
				int carry;
				while (y != 0){
					carry = x & y;
					x = x ^ y;
					y = carry << 1;
				}
				return x;
			}

			int bitwise_sub(int x, int y) {
				return bitwise_add(x, bitwise_negate(y));
			}

			int bitwise_mul(int x, int y) {
				int m = 1, z = 0;
				if (x<0) {
					x = bitwise_negate(x);
					y = bitwise_negate(y);	
				}
				while (x >= m && y) {
					if (x &m) z = bitwise_add(y, z);
					y <<= 1; m <<= 1;	
				}
				return z;
			}

			int bitwise_div(int x, int y) {
				int c = 0, sign = 0;

				if (x<0) {
					x = bitwise_negate(x);
					sign ^= 1;
				}

				if (y<0) {
					y = bitwise_negate(y);
					sign ^= 1;
				}

				if (y != 0) {
					while (x >= y) {
						x = bitwise_sub(x, y);
						++c;
					}
				}
				if (sign) {
					c = bitwise_negate(c);
				}
				return c;
			}

			XMath(){}
			~XMath(){}
			!XMath(){}
		private:
			int bitwise_negate(int x) {
				return bitwise_add(~x, 1);
			}
		};
	}
}
