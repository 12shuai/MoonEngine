#pragma once
#include <cstdint>
#include <ctype.h>
#include <cstring>
#include <cstdio>

namespace MOON {
	class MatchTool {
	public:
		inline static bool fuzzy_match_simple(char const *pattern, char const *str) {
			while (*pattern != '\0' && *str != '\0') {
				if (tolower(*pattern) == tolower(*str))
					++pattern;
				++str;
			}
			return *pattern == '\0' ? true : false;
		}

		inline static bool fuzzy_match(char const *pattern, char const *str, int &outScore) {
			uint8_t matches[256];
			return fuzzy_match(pattern, str, outScore, matches, sizeof(matches));
		}

		inline static bool fuzzy_match(char const *pattern, char const *str, int &outScore, uint8_t *matches, int maxMatches) {
			int recursionCount = 0;
			int recursionLimit = 10;
			return fuzzy_match_recursive(pattern, str, outScore, str, nullptr, matches, maxMatches, 0, recursionCount, recursionLimit);
		}

		inline static bool fuzzy_match_recursive(const char *pattern, const char *str, int &outScore,
			const char *strBegin, uint8_t const *srcMatches, uint8_t *matches, int maxMatches,
			int nextMatch, int & recursionCount, int recursionLimit) {
			++recursionCount;
			if (recursionCount >= recursionLimit)
				return false;

			if (*pattern == '\0' || *str == '\0')
				return false;

			bool recursiveMatch = false;
			uint8_t bestRecursiveMatches[256];
			int bestRecursiveScore = 0;
			bool first_match = true;

			while (*pattern != '\0' && *str != '\0') {
				if (tolower(*pattern) == tolower(*str)) {
					if (nextMatch >= maxMatches)
						return false;

					if (first_match && srcMatches) {
						memcpy(matches, srcMatches, nextMatch);
						first_match = false;
					}

					uint8_t recursiveMatches[256];
					int recursiveScore;

					if (fuzzy_match_recursive(pattern, str + 1, recursiveScore, strBegin, matches, recursiveMatches, sizeof(recursiveMatches), nextMatch, recursionCount, recursionLimit)) {
						if (!recursiveMatch || recursiveScore > bestRecursiveScore) {
							memcpy(bestRecursiveMatches, recursiveMatches, 256);
							bestRecursiveScore = recursiveScore;
						}
						recursiveMatch = true;
					}

					matches[nextMatch++] = (uint8_t)(str - strBegin);
					++pattern;
				}
				++str;
			}

			bool matched = *pattern == '\0' ? true : false;

			if (matched) {
				const int sequential_bonus = 15;
				const int separator_bonus = 30;
				const int camel_bonus = 30;
				const int first_letter_bonus = 15;
				const int leading_letter_penalty = -5;
				const int max_leading_letter_penalty = -15;
				const int unmatched_letter_penalty = -1;

				while (*str != '\0')
					++str;

				outScore = 100;

				int penalty = leading_letter_penalty * matches[0];
				if (penalty < max_leading_letter_penalty)
					penalty = max_leading_letter_penalty;

				outScore += penalty;

				int unmatched = (int)(str - strBegin) - nextMatch;
				outScore += unmatched_letter_penalty * unmatched;

				for (int i = 0; i < nextMatch; ++i) {
					uint8_t currIdx = matches[i];
					if (i > 0) {
						uint8_t prevIdx = matches[i - 1];
						if (currIdx == (prevIdx + 1))
							outScore += sequential_bonus;
					}

					if (currIdx > 0) {
						char neighbor = strBegin[currIdx - 1];
						char curr = strBegin[currIdx];
						if (::islower(neighbor) && ::isupper(curr))
							outScore += camel_bonus;
						bool neighborSeparator = neighbor == '_' || neighbor == ' ';
						if (neighborSeparator)
							outScore += separator_bonus;
					} else {
						outScore += first_letter_bonus;
					}
				}

			}

			if (recursiveMatch && (!matched || bestRecursiveScore > outScore)) {
				memcpy(matches, bestRecursiveMatches, maxMatches);
				outScore = bestRecursiveScore;
				return true;
			} else if (matched) {
				return true;
			} else {
				return false;
			}
		}

	};
}

/*
����ƥ����

���ʱ��ÿ��ǵ�ʲô���أ����ȣ������ǿ�����ƵĴ�ֵ㣺
ƥ�����ĸ
δƥ�����ĸ
����ƥ�����ĸ
�ӽ���ʼ
�ָ��������ĸ���ո�������ո��»��ߣ�
д��ĸ����Ĵ�д��ĸ������CamelCase���շ���������

�ⲿ�ֺܼ򵥡�ƥ�����ĸ�ӷ֡���ƥ����ĸ���֡�ƥ��ӽ���ʼ�ӷ֡�ƥ������м�ĵ�һ����ĸ�ӷ֡�
���շ�����������ƥ���д��ĸ�ӷ֡�
��Ȼ������ô�ӷ֣��Ӷ��ٷ֣�Ŀǰ�һ���֪����ȷ�Ĵ𰸡�Ȩ��ȡ�������Ԥ�����ݼ���
�ļ�·�����ļ�����ͬ���ļ���չ������Ժ��Եġ��������ʹ���������ƥ�䣬���������ָ��������մ�Сд��
Ŀǰ���Ƕ�����һ������ָ���Ȩ�⡣���Ժܶ಻ͬ�����ݼ����кܺõ�Ч����

������0��ʼ��
ƥ�����ĸ��+0��
��ƥ�����ĸ��-1��
����ƥ��ӷ֣�+5��
�ָ���ƥ��ӷ֣�+10��
�շ�ƥ��ӷ֣�+10��
��ƥ��Ĵ�д��ĸ��-3�֣����-9��

��Ҫָ�����Ǵ��ֵû��ɶʵ�ڵ����壬ֻ��Ϊһ����ԱȽϵĲο���
[�÷ַ�ΧҲû���޶���0-100���������-50-50֮��]

���ڲ�ƥ�����ĸ���֣��ϳ��ĵ��ʾ��п��ܻ�õ��Ƚϵ͵���ͷ�ֵ��
����ƥ��ӷ֣�����������ģʽ���ܸ����ܵõ���߷֡�

�ָ������շ�ӷֱȽϴ�������ƥ��ӷֱȽ������塣
����㲻ƥ��ǰ������ĸ������֡�����ڿ�ʼ����ƥ���ӷ֡��м�ͽ���֮���ƥ��û������
��ȫƥ��û����ȷ�ļӷֻ��ơ���ƥ�����ĸ�л���֡����Ը��̵��ַ����͸�����ƥ���÷ָ���
��ŵļӷ�����������������ڵ�������ģʽ��������԰���������
*/