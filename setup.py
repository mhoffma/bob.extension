#!/usr/bin/env python
# vim: set fileencoding=utf-8 :
# Andre Anjos <andre.anjos@idiap.ch>
# Thu 20 Sep 2012 14:43:19 CEST 

"""A package that contains a helper for Bob/Python C++ extension development
"""

from setuptools import setup, find_packages

setup(

    name="xbob.extension",
    version="0.1.0",
    description="Helps projects building Python/C++ extensions for Bob",
    license="GPLv3",
    author='Andre Anjos',
    author_email='andre.anjos@idiap.ch',
    long_description=open('README.rst').read(),
    url='http://pypi.python.org/pypi/xbob.extension',

    packages=find_packages(),

    namespace_packages=[
      "xbob",
      ],

    install_requires=[
      'setuptools',
      ],

    classifiers = [
      'Development Status :: 3 - Alpha',
      'Intended Audience :: Developers',
      'License :: OSI Approved :: GNU General Public License v3 (GPLv3)',
      'Natural Language :: English',
      'Programming Language :: Python',
      'Topic :: Software Development :: Libraries :: Python Modules',
      ],
    
    )
