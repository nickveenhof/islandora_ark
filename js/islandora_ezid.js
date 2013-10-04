/**
 * JavaScript functions for the Islandora EZID module.
 */

(function($) {
	Drupal.behaviors.IslandoraEZID = {
		attach : function(context, settings) {
			/* I don't seem to be able override this if I put it in CSS (?!) */
			$('#edit-ezidusername').css("color", "#bbb");
			
			/* Clear out the default value so users can enter their own. */
			$('#edit-ezidusername').focus(function() {
				if ($(this).val() === "YOUR_USERNAME") {
					$(this).val('');
					$(this).css("color", "black");
				}
			});
		}
	};
})(jQuery);
