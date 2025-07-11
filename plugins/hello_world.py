"""
Hello World Plugin for IGED
Sample plugin demonstrating the plugin system
"""


class Plugin:
    """Plugin implementation."""

    def __init__(self) -> None:
        """Init   function."""
        self.name = "Hello World"
        self.version = "1.0.0"
        self.description = "A simple hello world plugin"

    def run(self, input_text) -> None:
        """Run the plugin with input text"""
        return f"Hello, {input_text}! This is the Hello World plugin speaking."

    def get_info(self) -> None:
        """Get plugin information"""
        return {
            "name": self.name,
            "version": self.version,
            "description": self.description,
        }
