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

			perSystem = { pkgs, ... }:
				let
					tabulate = pkgs.stdenv.mkDerivation rec {
						pname = "tabulate";
						version = "1.5";

						nativeBuildInputs = with pkgs; [ cmake ];

						src = pkgs.fetchFromGitHub {
							owner = "p-ranav";
							repo = "tabulate";
							rev = "v${version}";
							hash = "sha256-A4fsNzRmfHxKx5nZNUkKWMVyHDLWV5R5U1HLh9iUrTE=";
						};

						meta = with pkgs.lib; {
							license = licenses.mit;
							homepage = "https://github.com/p-ranav/tabulate";
							platforms = platforms.linux ++ platforms.darwin;
						};
					};

					dual_sense_hid = pkgs.stdenv.mkDerivation (finalAttrs: {
						name = "dual_sense_hid";
						pname = "dual_sense_hid";

						src = ./.;
						nativeBuildInputs = with pkgs; [ cmake ];
						propagatedBuildInputs = with pkgs; [ hidapi ];

						cmakeFlags = [
							(pkgs.lib.cmakeBool "ENABLE_TESTS" finalAttrs.finalPackage.doCheck)
						];

						doCheck = true;
						checkInputs = [ pkgs.gtest ];
					});
				in
				{
					packages = {
						default = dual_sense_hid;
					};

					devShells.default = pkgs.mkShell {
						inputsFrom = [ dual_sense_hid ];

						packages = with pkgs; [
							fmt
							tabulate
						];
					}
					// pkgs.lib.optionalAttrs pkgs.stdenv.hostPlatform.isDarwin {
						PATH_LOCALE = "${pkgs.darwin.locale}/share/locale";
					}
					// pkgs.lib.optionalAttrs pkgs.stdenv.hostPlatform.isLinux {
						LOCALE_ARCHIVE = "${pkgs.glibcLocales}/lib/locale/locale-archive";
					}
;
				};
		};
}