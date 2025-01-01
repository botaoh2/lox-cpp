import unittest
import subprocess

def run_script(script):
    command = ['build/debug/lox', 'test/' + script]
    return subprocess.run(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)

def read_file(file):
    with open('test/' + file) as file:
        return file.read()

class Basic(unittest.TestCase):

    def test_print(self):
        result = run_script('print.lox')
        
        self.assertEqual(result.returncode, 0)
        self.assertEqual(result.stdout, 'one\ntrue\n3\n')
        self.assertEqual(result.stderr, '')
    
    def test_varstmt(self):
        result = run_script('varstmt.lox')

        self.assertEqual(result.returncode, 0)
        self.assertEqual(result.stdout, '3\n')
        self.assertEqual(result.stderr, '')
    
    def test_varempty(self):
        result = run_script('varempty.lox')

        self.assertEqual(result.returncode, 0)
        self.assertEqual(result.stdout, 'nil\n')
        self.assertEqual(result.stderr, '')

    def test_assignment(self):
        result = run_script('assignment.lox')

        self.assertEqual(result.returncode, 0)
        self.assertEqual(result.stdout, '1\n1\n2\n2\n')
        self.assertEqual(result.stderr, '')

    def test_scope(self):
        result = run_script('scope.lox')

        self.assertEqual(result.returncode, 0)
        self.assertEqual(result.stdout, read_file('scope.txt'))
        self.assertEqual(result.stderr, '')

if __name__ == '__main__':
    unittest.main()
