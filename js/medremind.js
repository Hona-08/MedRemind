/**
 * @file
 * JavaScript for MedRemind module.
 */

(function ($) {
  // ↑ Wrap everything in a closure
  //   Drupal uses jQuery but other libraries might use $ too
  //   This way $ is safely = jQuery inside this function

  Drupal.behaviors.medremind = {
    // ↑ Drupal.behaviors = Drupal's way of running JavaScript
    //   Unlike $(document).ready(), behaviors run AGAIN when
    //   new content loads via AJAX. This is very important!

    attach: function (context, settings) {
      // ↑ attach() runs when:
      //   1. Page first loads
      //   2. New content added via AJAX
      //   context = the newly added HTML
      //   settings = Drupal.settings (PHP can pass data here)

      $('.medremind-welcome', context).once('medremind', function () {
        // ↑ .once('id') = run this code only ONCE per element
        //   Prevents duplicate event handlers on AJAX reload
        //   'medremind' = unique identifier

        console.log('MedRemind module loaded successfully!');
      });
    }
  };

})(jQuery);
// ↑ Pass jQuery as the $ parameter