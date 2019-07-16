from setuptools import setup

setup(name="pyspatzsim",
      version="1.3",
      description="Python bindings for SpatzSim",
      url="https://git.spatz.wtf/spatzenhirn/Simulator",
      author="Jona Ruof",
      author_email="jona.ruof@uni-ulm.de",
      license="MIT",
      zip_safe=True,
      packages=[''],
      package_dir={'': 'build'},
      package_data={'': ["pyspatzsim.cpython-36m-x86_64-linux-gnu.so"]},
    )

