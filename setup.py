import pathlib
from setuptools import setup

HERE = pathlib.Path(__file__).parent

README = (HERE / "README.md").read_text()

setup(name='Vixpy',
      version='1.0.0',
      description='Python classes and bindings for VMWare VIX API',
      long_description=README,
      long_description_content_type="text/markdown",
      author='Doug S',
      url='https://github.com/dshikashio/vixpy',
      license="MIT",
      keywords = ['vmware', 'vix', 'automation'],
      classifiers=[
          'License :: OSI Approved :: MIT License',
          'Programming Language :: Python :: 3',
          'Development Status :: 5 - Production/Stable',
          'Intended Audience :: Developers',
          'Intended Audience :: Science/Research',
          'Natural Language :: English',
          'Programming Language :: Python',
          'Topic :: Software Development :: Libraries :: Python Modules',
      ],
      packages=['vixpy'],
      python_requires=">=3.6.0",
 )
