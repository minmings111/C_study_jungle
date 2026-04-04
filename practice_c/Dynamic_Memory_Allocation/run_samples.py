#!/usr/bin/env python3
import sys
from pathlib import Path

REPO = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(REPO / 'scripts'))
from q_runner_common import parse_args, run_cases

CASES = {
    'Data-Structures/Dynamic_Memory_Allocation/Q1_DMA.c': {'input': '42\n', 'expected': ['동적 값: 42']},
    'Data-Structures/Dynamic_Memory_Allocation/Q2_DMA.c': {'input': '5\n', 'expected': ['초기 배열: 0 0 0 0 0']},
    'Data-Structures/Dynamic_Memory_Allocation/Q3_DMA.c': {'input': '4\n1 3 5 7\n', 'expected': ['복제 배열: 1 3 5 7']},
    'Data-Structures/Dynamic_Memory_Allocation/Q4_DMA.c': {'input': 'pointer\n', 'expected': ['복제 문자열: pointer']},
    'Data-Structures/Dynamic_Memory_Allocation/Q5_DMA.c': {'input': '3\n1 2 3\n9\n', 'expected': ['확장 배열: 1 2 3 9']},
    'Data-Structures/Dynamic_Memory_Allocation/Q6_DMA.c': {'input': '6\n1 2 3 4 5 6\n', 'expected': ['짝수 개수: 3', '짝수 배열: 2 4 6']},
    'Data-Structures/Dynamic_Memory_Allocation/Q7_DMA.c': {'input': '2 3\n1 2 3 4 5 6\n', 'expected': ['행 0 합: 6', '행 1 합: 15']},
    'Data-Structures/Dynamic_Memory_Allocation/Q8_DMA.c': {'input': '2 3\n1 2 3 4 5 6\n', 'expected': ['행 0: 1 4', '행 1: 2 5', '행 2: 3 6']},
    'Data-Structures/Dynamic_Memory_Allocation/Q9_DMA.c': {'input': '3\nred blue green\n', 'expected': ['복제 단어들: red blue green']},
    'Data-Structures/Dynamic_Memory_Allocation/Q10_DMA.c': {'input': '3\nhello dynamic world\n', 'expected': ['이어 붙인 문자열: hello dynamic world']},
}

if __name__ == '__main__':
    args = parse_args('Dynamic_Memory_Allocation 샘플 실행 및 기본 채점')
    raise SystemExit(run_cases(CASES, target=args.target, config=args.config))
