import unittest
import subprocess

BUILD_FOLDER = 'build/debug/'
TEST_FOLDER = 'test/'

def run_script(script):
    command = [BUILD_FOLDER + 'lox', TEST_FOLDER + script]
    return subprocess.run(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)

def read_file(file):
    with open(TEST_FOLDER + file) as file:
        return file.read()

class Basic(unittest.TestCase):

    def test_print(self):
        result = run_script('print.lox')
        
        self.assertEqual(result.returncode, 0)
        self.assertEqual(result.stdout, read_file('print.txt'))
        self.assertEqual(result.stderr, '')

    def test_var(self):
        result = run_script('var.lox')

        self.assertEqual(result.returncode, 0)
        self.assertEqual(result.stdout, read_file('var.txt'))
        self.assertEqual(result.stderr, '')

    def test_assignment(self):
        result = run_script('assignment.lox')

        self.assertEqual(result.returncode, 0)
        self.assertEqual(result.stdout, read_file('assignment.txt'))
        self.assertEqual(result.stderr, '')

    def test_scope(self):
        result = run_script('scope.lox')

        self.assertEqual(result.returncode, 0)
        self.assertEqual(result.stdout, read_file('scope.txt'))
        self.assertEqual(result.stderr, '')
    
    def test_if(self):
        result = run_script('if.lox')

        self.assertEqual(result.returncode, 0)
        self.assertEqual(result.stdout, read_file('if.txt'))
        self.assertEqual(result.stderr, '')
    
    def test_logical(self):
        result = run_script('logical.lox')

        self.assertEqual(result.returncode, 0)
        self.assertEqual(result.stdout, read_file('logical.txt'))
        self.assertEqual(result.stderr, '')
    
    def test_while(self):
        result = run_script('while.lox')

        self.assertEqual(result.returncode, 0)
        self.assertEqual(result.stdout, read_file('while.txt'))
        self.assertEqual(result.stderr, '')
    
    def test_for(self):
        result = run_script('for.lox')

        self.assertEqual(result.returncode, 0)
        self.assertEqual(result.stdout, read_file('for.txt'))
        self.assertEqual(result.stderr, '')
    
    def test_operator(self):
        result = run_script('operator.lox')

        self.assertEqual(result.returncode, 0)
        self.assertEqual(result.stdout, read_file('operator.txt'))
        self.assertEqual(result.stderr, '')

if __name__ == '__main__':
    unittest.main()
