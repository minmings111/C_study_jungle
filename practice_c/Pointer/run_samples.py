#!/usr/bin/env python3
import sys
from pathlib import Path

REPO = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(REPO / 'scripts'))
from q_runner_common import parse_args, run_cases

CASES = {
    'practice_c/Pointer/Q1_PTR.c': {'input': '3 7\n', 'expected': ['교환 후 값: 7 3']},
    'practice_c/Pointer/Q2_PTR.c': {'input': '10 4\n', 'expected': ['합: 14', '차: 6']},
    'practice_c/Pointer/Q3_PTR.c': {'input': '5\n3 8 2 9 4\n', 'expected': ['최댓값: 9', '인덱스: 3']},
    'practice_c/Pointer/Q4_PTR.c': {'input': '5\n1 2 3 4 5\n', 'expected': ['뒤집은 배열: 5 4 3 2 1']},
    'practice_c/Pointer/Q5_PTR.c': {'input': 'banana a\n', 'expected': ['개수: 3']},
    'practice_c/Pointer/Q6_PTR.c': {'input': '5\n1 2 9 4 5\n1 2 3 4 5\n', 'expected': ['첫 불일치 인덱스: 2']},
    'practice_c/Pointer/Q7_PTR.c': {'input': '10 20\n', 'expected': ['포인터가 가리키는 값: 20 10']},
    'practice_c/Pointer/Q8_PTR.c': {'input': '5\n40 10 30 20 50\n', 'expected': ['정렬된 포인터 값: 10 20 30 40 50']},
    'practice_c/Pointer/Q9_PTR.c': {'input': 'abracadabra cad\n', 'expected': ['시작 인덱스: 4']},
    'practice_c/Pointer/Q10_PTR.c': {'input': 'red,blue,green\n', 'expected': ['토큰 1: red', '토큰 2: blue', '토큰 3: green']},
    'practice_c/Pointer/Q11_PTR.c': {'input': 'alpha beta\n', 'expected': ['교환 후 문자열: beta alpha']},
    'practice_c/Pointer/Q12_PTR.c': {'input': '4\ncat elephant dog bird\n', 'expected': ['가장 긴 단어 인덱스: 1', '가장 긴 단어: elephant']},
    'practice_c/Pointer/Q13_PTR.c': {'input': '3\n1 2 3\n4 5 6\n7 8 9\n', 'expected': ['주대각선 합: 15']},
    'practice_c/Pointer/Q14_PTR.c': {'input': '5 4\nred apple go banana sky\n', 'expected': ['긴 단어 1: apple', '긴 단어 2: banana']},
    'practice_c/Pointer/Q15_PTR.c': {'input': '2 3\n1 2 3\n4 5 6\n1 2\n', 'expected': ['선택한 값: 6']},
}

if __name__ == '__main__':
    args = parse_args('Pointer 샘플 실행 및 기본 채점')
    raise SystemExit(run_cases(CASES, target=args.target, config=args.config))
