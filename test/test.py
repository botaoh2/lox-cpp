import unittest
import subprocess

def run_script(script):
    command = ['build/debug/lox', script]
    return subprocess.run(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)

class Basic(unittest.TestCase):

    def test_print(self):
        result = run_script('test/print.lox')
        
        self.assertEqual(result.returncode, 0)
        self.assertEqual(result.stdout, 'one\ntrue\n3\n')
        self.assertEqual(result.stderr, '')

if __name__ == '__main__':
    unittest.main()
