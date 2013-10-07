/**
 * JavaScript functions for the Islandora EZID module.
 */

(function($) {
	Drupal.behaviors.IslandoraEZID = {
		attach : function(context, settings) {
			if ($(this).val() === "YOUR_USERNAME") {
				$('#edit-islandora-ezid-settings-ezidusername').css("color",
						"#bbb");
			}

			// We don't reload this form with changed data so this is sufficient
			$('#edit-arkcollectionname').css('color', '#bbb');

			/* Clear out the default value so users can enter their own. */
			$('#edit-islandora-ezid-settings-ezidusername').focus(function() {
				if ($(this).val() === "YOUR_USERNAME") {
					$(this).val('');
					$(this).css("color", "black");
				}
			});

			/* Clear out the default value so users can enter their own. */
			$('#edit-arkcollectionname').focus(function() {
				if ($(this).val() === "COLLECTION_NAME") {
					$(this).val('');
					$(this).css("color", "black");
				}
			});
		}
	};
})(jQuery);
