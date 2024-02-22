{
    description = "DIY Dual Sense support via HID interface";

    inputs = {
        nixpkgs.url = "github:NixOs/nixpkgs/release-23.11";
        flake-parts.url = "github:hercules-ci/flake-parts";
    };

    outputs = inputs@{ nixpkgs, flake-parts, ... }:
        flake-parts.lib.mkFlake { inherit inputs; } {
            systems = [
                "x86_64-darwin"
                "aarch64-darwin"
                "x86_64-linux"
                "i686-linux"
                "aarch64-linux"
            ];

            perSystem = { pkgs, ... }: {
                packages.default = pkgs.stdenv.mkDerivation {
                    name = "dual_sense_hid";
                    pname = "dual_sense_hid";

                    src = ./.;
                    nativeBuildInputs = [
                        pkgs.cmake

                        pkgs.hidapi
                    ];
                };
            };
        };
}