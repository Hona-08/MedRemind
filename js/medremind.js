/**
 * @file
 * JavaScript for MedRemind module.
 * Handles AJAX Take/Skip actions and UI updates.
 */

(function ($) {

  Drupal.behaviors.medremind = {
    attach: function (context, settings) {

      // ============================================
      // TAKE BUTTON — AJAX Handler
      // ============================================

      $('.med-btn-take', context).once('medremind-take', function () {
        // ↑ .once() prevents binding the click event multiple times

        $(this).click(function (e) {
          e.preventDefault();
          // ↑ Prevent default link behavior (don't navigate to URL)

          var $btn = $(this);
          var $card = $btn.closest('.med-card');
          // ↑ .closest() finds the parent .med-card element
          var medId = $card.data('med-id');
          // ↑ Read the data-med-id attribute from the card
          var url = $btn.attr('href');
          // ↑ The AJAX URL from the href attribute

          // Disable button to prevent double clicks
          $btn.addClass('disabled').text('Taking...');

          // Send AJAX request
          $.ajax({
            url: url,
            type: 'GET',
            dataType: 'json',
            // ↑ Expect JSON response

            success: function (response) {
              // ↑ Called when server returns successfully

              if (response.status === 'ok') {

                // Update the card to show "Taken" state
                $card.addClass('med-card-taken');
                // ↑ Add green background class

                // Replace buttons with "Taken" badge
                $card.find('.med-card-actions').html(
                  '<span class="med-taken-badge">✅ Taken</span>'
                );

                // Update the icon
                $card.find('.med-card-icon').text('✅');

                // Update streak if returned
                if (response.streak > 0) {
                  var streakHtml = '<div class="med-card-streak">🔥 ' +
                    response.streak + ' day streak</div>';

                  // Remove old streak and add new
                  $card.find('.med-card-streak').remove();
                  $card.find('.med-card-manage').before(streakHtml);
                }

                // Show success message at top of page
                _medremindShowMessage(response.message, 'status');

                // Update stats bar
                _medremindUpdateStats('taken');
              }
            },

            error: function () {
              // ↑ Called if request fails
              $btn.removeClass('disabled').text('✅ Take');
              _medremindShowMessage('Something went wrong. Please try again.', 'error');
            }
          });
        });
      });


      // ============================================
      // SKIP BUTTON — AJAX Handler
      // ============================================

      $('.med-btn-skip', context).once('medremind-skip', function () {

        $(this).click(function (e) {
          e.preventDefault();

          var $btn = $(this);
          var $card = $btn.closest('.med-card');
          var url = $btn.attr('href');

          // Confirm before skipping
          if (!confirm('Are you sure you want to skip this dose?')) {
            return;
            // ↑ If user clicks "Cancel" on the confirm dialog, do nothing
          }

          $btn.addClass('disabled').text('Skipping...');

          $.ajax({
            url: url,
            type: 'GET',
            dataType: 'json',

            success: function (response) {
              if (response.status === 'ok') {

                // Show skipped state
                $card.find('.med-card-actions').html(
                  '<span class="med-taken-badge" style="background:#fff3e0;color:#e67e22;">⏭️ Skipped</span>'
                );

                // Remove streak (skipping breaks it)
                $card.find('.med-card-streak').remove();

                _medremindShowMessage(response.message, 'warning');
              }
            },

            error: function () {
              $btn.removeClass('disabled').text('⏭️ Skip');
              _medremindShowMessage('Something went wrong. Please try again.', 'error');
            }
          });
        });
      });

    }
  };


  // ============================================
  // HELPER FUNCTIONS
  // ============================================

  /**
   * Show a message at the top of the content area.
   * Like drupal_set_message() but without page reload.
   */
  function _medremindShowMessage(message, type) {
    // Remove any existing messages first
    $('.medremind-ajax-message').remove();

    var cssClass = 'messages status';
    if (type === 'error') {
      cssClass = 'messages error';
    }
    else if (type === 'warning') {
      cssClass = 'messages warning';
    }

    var $msg = $('<div class="medremind-ajax-message ' + cssClass + '">' + message + '</div>');

    // Insert at top of content area
    $('.medremind-dashboard').prepend($msg);

    // Auto-hide after 4 seconds
    setTimeout(function () {
      $msg.fadeOut(500, function () {
        $(this).remove();
      });
    }, 4000);
    // ↑ fadeOut(500) = fade out over 500 milliseconds
    //   then remove the element completely
  }

  /**
   * Update the stats bar numbers without page reload.
   */
  function _medremindUpdateStats(action) {
    if (action === 'taken') {
      var $taken = $('.stat-taken .stat-number');
      var current = parseInt($taken.text()) || 0;
      // ↑ parseInt() converts "0" string to 0 number
      $taken.text(current + 1);
      // ↑ Increment taken count by 1

      // Quick animation on the number
      $taken.css('color', '#27ae60').animate({fontSize: '36px'}, 200)
        .animate({fontSize: '32px'}, 200);
      // ↑ Brief "pop" animation: grow then shrink
    }
  }

})(jQuery)