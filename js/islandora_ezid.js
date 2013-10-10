/**
 * JavaScript functions for the Islandora EZID module.
 */

(function($) {
	Drupal.behaviors.IslandoraEZID = {
		attach : function(context, settings) {
			// This is from the admin EZID ARK settings form
			if ($('#edit-islandora-ezid-settings-ezidusername').val() === 'YOUR_USERNAME') {
				$('#edit-islandora-ezid-settings-ezidusername').css('color', '#bbb');
			}

			// We don't reload the batch form with changed data so this is sufficient
			$('#edit-arktagsdefault').css('color', '#bbb');

			/* Clear out the default value so users can enter their own. */
			$('#edit-islandora-ezid-settings-ezidusername').focus(function() {
				if ($(this).val() === 'YOUR_USERNAME') {
					$(this).val('');
					$(this).css('color', 'black');
				}
			});

			/* Clear out the default value so users can enter their own. */
			$('#edit-arktagsdefault').focus(function() {
				if ($(this).val() === 'COLLECTION_NAME') {
					$(this).val('');
					$(this).css('color', 'black');
				}
			});
		}
	};
})(jQuery);
