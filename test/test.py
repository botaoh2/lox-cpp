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

if __name__ == '__main__':
    unittest.main()
