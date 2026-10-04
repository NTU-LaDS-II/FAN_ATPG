#!/usr/bin/env python3
"""Check report_fault using only the repository's bundled SAF workloads.

Run after make: python3 tests/report_fault_scan.py
Optionally compare statistics with an unmodified executable: --baseline /path/to/fan
"""
import argparse
from collections import Counter
from pathlib import Path
import re
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[1]
DESIGNS = ('s27', 's5378', 's9234', 's15850')
STATES = ('UD', 'DT', 'PT', 'AU', 'TI', 'RE', 'AB')


def require(condition, message):
    if not condition:
        raise AssertionError(message)


def run(fan, design, report_command=None):
    commands = ['read_lib techlib/mod_nangate45.mdt',
                'read_netlist mod_netlist/' + design + '.v',
                'build_circuit --frame 1', 'read_pattern pat/FAN_' + design + '.pat',
                'set_fault_type saf', 'add_fault -a', 'run_fault_sim', 'report_statistics']
    if report_command:
        commands.append(report_command)
    with tempfile.TemporaryDirectory(prefix='fan-report-regression-') as temporary:
        script = Path(temporary) / 'replay.script'
        script.write_text('\n'.join(commands + ['exit']) + '\n', encoding='utf-8')
        result = subprocess.run([str(fan), '-f', str(script)], cwd=ROOT,
                                capture_output=True, text=True, timeout=120)
    require(result.returncode == 0, design + ': FAN exited ' + str(result.returncode)
            + '\n' + result.stderr)
    require('**ERROR' not in result.stdout + result.stderr, design + ': FAN command failed')
    return result.stdout


def statistics(text):
    def value(expression):
        match = re.search(expression, text)
        require(match is not None, 'missing statistic: ' + expression)
        return match[1]
    return dict(full=int(value(r'FU \(full\)\s+(\d+)')),
                collapsed=int(value(r'FU \(collapsed\)\s+(\d+)')),
                patterns=int(value(r'#Patterns\s+(\d+)')),
                coverage=value(r'fault coverage\s+([\d.]+)%'),
                states={state: int(value(state + r' \([^\n]+\)\s+(\d+)'))
                        for state in STATES})


def records(text):
    rows = re.findall(r'^#\s+(SA[01])\s+(UD|DT|PT|AU|TI|RE|AB)\s+'
                      r'(.+?)\s+\[equivalent=(\d+)\]\s*$', text, re.M)
    result = {}
    for kind, state, site, weight in rows:
        require(not site.startswith('('), 'anonymous scan fault site: ' + site)
        identity = (kind, site)
        require(identity not in result, 'duplicate identity: ' + str(identity))
        require(int(weight) > 0, 'nonpositive equivalence multiplicity')
        result[identity] = (state, int(weight))
    return result


def check(fan, baseline, design):
    full_text = run(fan, design, 'report_fault')
    stats = statistics(full_text)
    full = records(full_text)
    require(len(full) == stats['collapsed'],
            design + ': full export has ' + str(len(full)) + ' rows; expected '
            + str(stats['collapsed']))
    require(len(full) == int(re.search(r'number of faults:\s*(\d+)', full_text)[1]),
            design + ': declared export size differs')
    totals = Counter()
    for state, weight in full.values():
        totals[state] += weight
    require(sum(totals.values()) == stats['full'], design + ': full fault weight differs')
    require(all(totals[state] == stats['states'][state] for state in STATES),
            design + ': state weights differ from statistics')
    for state in ('DT', 'UD'):
        filtered_text = run(fan, design, 'report_fault -s ' + state)
        filtered = records(filtered_text)
        require(statistics(filtered_text) == stats, design + ': filtered statistics changed')
        require(filtered == {key: value for key, value in full.items() if value[0] == state},
                design + ': filtered identities/states/weights differ for ' + state)
    for control in ('CK', 'test_si', 'test_so', 'test_se'):
        for kind in ('SA0', 'SA1'):
            require((kind, control) in full, design + ': missing control fault ' + control)
    if design == 's27':
        for cell in ('U_G5', 'U_G6', 'U_G7'):
            for port in ('Q', 'D'):
                for kind in ('SA0', 'SA1'):
                    require((kind, cell + '/' + port + ' (SDFF_X1)') in full,
                            'missing functional scan fault: ' + cell + '/' + port)
    if baseline:
        require(statistics(run(baseline, design)) == stats,
                design + ': simulation statistics differ from unmodified executable')
    print('PASS ' + design + ': ' + str(len(full)) + ' unique collapsed classes; '
          + str(stats['full']) + ' full faults; DT/UD filters and scan/control sites', flush=True)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--fan', type=Path, default=ROOT / 'bin/opt/fan')
    parser.add_argument('--baseline', type=Path)
    parser.add_argument('--designs', nargs='+', choices=DESIGNS, default=DESIGNS)
    args = parser.parse_args()
    fan = args.fan.resolve()
    baseline = args.baseline.resolve() if args.baseline else None
    for executable in (fan, baseline):
        if executable is not None:
            require(executable.is_file(), 'executable missing: ' + str(executable))
    for design in args.designs:
        check(fan, baseline, design)


if __name__ == '__main__':
    main()