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

    def test_clock(self):
        result = run_script('clock.lox')

        self.assertEqual(result.returncode, 0)
        self.assertEqual(result.stderr, '')

        clock = float(result.stdout)
        self.assertTrue(0 <= clock <= 1)
    
    def test_function(self):
        result = run_script('function.lox')

        self.assertEqual(result.returncode, 0)
        self.assertEqual(result.stdout, read_file('function.txt'))
        self.assertEqual(result.stderr, '')
    
    def test_fib(self):
        result = run_script('fib.lox')

        self.assertEqual(result.returncode, 0)
        self.assertEqual(result.stdout, read_file('fib.txt'))
        self.assertEqual(result.stderr, '')
    
    def test_closure(self):
        result = run_script('closure.lox')

        self.assertEqual(result.returncode, 0)
        self.assertEqual(result.stdout, read_file('closure.txt'))
        self.assertEqual(result.stderr, '')
    
    def test_resolve(self):
        result = run_script('resolve.lox')

        self.assertEqual(result.returncode, 0)
        self.assertEqual(result.stdout, read_file('resolve.txt'))
        self.assertEqual(result.stderr, '')
    
    def test_class(self):
        result = run_script('class.lox')

        self.assertEqual(result.returncode, 0)
        self.assertEqual(result.stdout, read_file('class.txt'))
        self.assertEqual(result.stderr, '')
    
    def test_cake(self):
        result = run_script('cake.lox')

        self.assertEqual(result.returncode, 0)
        self.assertEqual(result.stdout, read_file('cake.txt'))
        self.assertEqual(result.stderr, '')

if __name__ == '__main__':
    unittest.main()
