import os
import sys
import subprocess

from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext


class CMakeExtension(Extension):
    def __init__(self, name, sourcedir=''):
        Extension.__init__(self, name, sources=[])
        self.sourcedir = os.path.abspath(sourcedir)


class CMakeBuild(build_ext):
    def run(self):
        try:
            out = subprocess.check_output(['cmake', '--version'])
        except OSError:
            raise RuntimeError(
                "CMake must be installed to build the following extensions: " +
                ", ".join(e.name for e in self.extensions))

        for ext in self.extensions:
            self.build_extension(ext)

    def build_extension(self, ext):
        extdir = os.path.abspath(os.path.dirname(self.get_ext_fullpath(ext.name)))
        # required for auto-detection of auxiliary "native" libs
        if not extdir.endswith(os.path.sep):
            extdir += os.path.sep

        cmake_args = ['-DCMAKE_LIBRARY_OUTPUT_DIRECTORY=' + extdir,
                      '-DPYTHON_EXECUTABLE=' + sys.executable]

        cfg = 'Debug' if self.debug else 'Release'
        build_args = ['--config', cfg]

        cmake_args += ['-DCMAKE_BUILD_TYPE=' + cfg]
        build_args += ['--', '-j2']

        env = os.environ.copy()
        env['CXXFLAGS'] = '{} -DVERSION_INFO=\\"{}\\"'.format(
            env.get('CXXFLAGS', ''),
            self.distribution.get_version())
        if not os.path.exists(self.build_temp):
            os.makedirs(self.build_temp)

        # CMakeLists.txt is in the same directory as this setup.py file
        cmake_list_dir = os.path.abspath(os.path.dirname(__file__))
        print('-' * 10, 'Running CMake prepare', '-' * 40)
        # subprocess.check_call(['cmake', ext.sourcedir] + cmake_args,
        # cwd=self.build_temp, env=env)
        subprocess.check_call(['cmake', cmake_list_dir] + cmake_args,
                              cwd=self.build_temp, env=env)

        print('-' * 10, 'Building extensions', '-' * 40)
        #cmake_cmd = ['cmake', '--build', '.'] + self.build_args
        subprocess.check_call(['cmake', '--build', '.'] + build_args,
                              cwd=self.build_temp)

setup(
    name='pycomdensity',
    version='0.1.1',
    author='Gregor Weiss',
    author_email='gregor.weiss@phys.chem.ethz.ch',
    description='This package contains density based clustering routes of the common nearest neighbor type.',
    long_description='',
    ext_modules=[CMakeExtension('pycomdensity')],
    cmdclass=dict(build_ext=CMakeBuild),
    zip_safe=False
)
