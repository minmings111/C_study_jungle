#!/usr/bin/env python3
import argparse
import subprocess
import tempfile
from pathlib import Path


def parse_args(description):
    parser = argparse.ArgumentParser(description=description)
    parser.add_argument("target", nargs="?", help="특정 C 파일만 실행")
    parser.add_argument("--config", help="reserved option for compatibility")
    return parser.parse_args()


def _repo_root():
    return Path(__file__).resolve().parents[1]


def _run_one(source, case):
    repo = _repo_root()
    source_path = repo / source
    if not source_path.exists():
      print(f"[ERROR] 파일을 찾을 수 없습니다: {source}")
      return False

    with tempfile.TemporaryDirectory() as tmpdir:
        binary = Path(tmpdir) / source_path.stem
        compile_proc = subprocess.run(
            ["gcc", str(source_path), "-o", str(binary)],
            capture_output=True,
            text=True,
            cwd=repo,
        )
        if compile_proc.returncode != 0:
            print(f"[FAIL] {source} 컴파일 실패")
            if compile_proc.stderr.strip():
                print(compile_proc.stderr.strip())
            return False

        run_proc = subprocess.run(
            [str(binary)],
            input=case["input"],
            capture_output=True,
            text=True,
            cwd=repo,
        )
        output = run_proc.stdout.replace("\r\n", "\n")
        ok = all(expected in output for expected in case["expected"])

        if ok:
            print(f"[PASS] {source}")
            return True

        print(f"[FAIL] {source}")
        print("expected:")
        for expected in case["expected"]:
            print(f"  - {expected}")
        print("actual:")
        actual = output.strip() or "(no stdout)"
        print(actual)
        if run_proc.stderr.strip():
            print("stderr:")
            print(run_proc.stderr.strip())
        return False


def run_cases(cases, target=None, config=None):
    del config
    selected = cases
    if target:
        target_path = target.replace("\\", "/")
        selected = {k: v for k, v in cases.items() if k.endswith(target_path) or k == target_path}
        if not selected:
            print(f"[ERROR] 대상 케이스를 찾을 수 없습니다: {target}")
            return 1

    failures = 0
    for source, case in selected.items():
        if not _run_one(source, case):
            failures += 1

    if failures == 0:
        print(f"\n모든 테스트 통과: {len(selected)}개")
        return 0

    print(f"\n실패: {failures}개 / 전체: {len(selected)}개")
    return 1
