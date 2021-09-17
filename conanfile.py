import os

from QtToolsFish import conans_tools
from QtToolsFish.Conans import QtConanFile
from conans import tools

package_name = "Geometry"
package_version = "develop"

package_user_channel = "cmguo/test"


class ConanConfig(QtConanFile):
    name = package_name
    version = package_version

    git_url = "git@git.100tal.com:epg_xhb_solution/talcloud_khaos_geometry.git"
    git_branch = "develop/master"

    description = "Geometry Library"

    requires = "QtComposition/master@cmguo/stable", "qtpromise/v0.5.0@cmguo/stable", "ShowBoard/develop@cmguo/test"

    exports_sources = "*"

    def source(self):
        conans_tools.move_dir_files_to_folder(self.get_library_name())
        super(ConanConfig, self).source()


if __name__ == '__main__':
    conans_tools.remove_cache(package_version=f"{package_name}/{package_version}", user_channel=package_user_channel)
    conans_tools.create(user_channel=package_user_channel)
    conans_tools.upload(package_version=f"{package_name}/{package_version}", user_channel=package_user_channel)
